#include <cstdlib>
#include <iostream>
#include "helper.hpp"
#include "soundplayer.hpp"
#include "brick.hpp"
#include "constants.hpp"
#include "rapidcsv.h"
#include <charconv>
#include <spdlog/spdlog.h>

using namespace std::literals;

sf::Image& getImage(brick::BrickProperty property)
{
    static bool initialized = false;
    static std::vector<sf::Image> list;
    if (!initialized) {
        sf::Image source;
        if (!source.loadFromFile((constants::resouces_path / "brick.png").string())) {
            const std::string message =
                "Cannot open source image "s + (constants::resouces_path / "brick.png").string();
            throw std::logic_error(message.c_str());
        }

        const auto width = source.getSize().x;
        const auto height = source.getSize().y;
        const auto* const pixels = source.getPixelsPtr();
        std::vector<std::vector<uint8_t>> matrix(width / constants::brick_width);

        for (unsigned int px_y = 0; px_y < height; ++px_y) {
            for (unsigned int px_x = 0; px_x < width; ++px_x) {
                auto index = (px_x + px_y * width) * 4; // 4 bytes per pixel (RGBA)
                auto index_matrix = px_x / constants::brick_width;
                matrix.at(index_matrix).push_back(pixels[index]);
                matrix.at(index_matrix).push_back(pixels[index + 1]);
                matrix.at(index_matrix).push_back(pixels[index + 2]);
                matrix.at(index_matrix).push_back(pixels[index + 3]);
            }
        }

        for (const auto& elm : matrix) {
            list.emplace_back(sf::Vector2u{constants::brick_width, constants::brick_height}, elm.data());
        }

        initialized = true;
    }

    return list.at(static_cast<size_t>(property) - size_t(1));
}

sf::Texture& brick::get_texture(BrickProperty property)
{
    static sf::Texture empty;
    static sf::Texture brick;
    static sf::Texture diamond;
    static sf::Texture bomb;
    static sf::Texture scaleup;
    static sf::Texture clone;
    static bool initialized = false;
    static int retryCount = 0;
    while (!initialized) {
        try {
            if (!brick.loadFromImage(getImage(BRICK))) {
                throw std::logic_error("Get texture brick from image data has failed\n");
            }
            if (!diamond.loadFromImage(getImage(DIAMOND))) {
                throw std::logic_error("Get texture diamond from image data has failed\n");
            }
            if (!bomb.loadFromImage(getImage(BOMB))) {
                throw std::logic_error("Get texture bomb from image data has failed\n");
            }
            if (!scaleup.loadFromImage(getImage(SCALEUP))) {
                throw std::logic_error("Get texture scaleup from image data has failed\n");
            }
            if (!clone.loadFromImage(getImage(CLONE))) {
                throw std::logic_error("Get texture clone from image data has failed\n");
            }
        }
        catch (const std::exception& e) {
            spdlog::error("Failed to load brick textures: {}", e.what());
            if (retryCount < 2) {
                spdlog::warn("Retrying texture loading after 1 second... (attempt {}/{})", retryCount + 1, 2);
                retryCount++;
                sf::sleep(sf::seconds(1.0f));
                continue;
            }
            throw;
        }

        initialized = true;
    }
    switch (property) {
    case BRICK:
        return brick;
    case DIAMOND:
        return diamond;
    case BOMB:
        return bomb;
    case SCALEUP:
        return scaleup;
    case CLONE:
        return clone;
    case NONE:
        [[fallthrough]];
    default:
        return empty;
    }
}

brick::brick(float px_x, float px_y, BrickProperty property) : m_wall(nullptr), m_property(property), m_hitCount(0)
{
    set_texture(get_texture(property));
    brick::init(px_x, px_y);
}

void brick::init(float px_x, float px_y) noexcept
{
    set_position({px_x, px_y});
}

// Compute the brick's new position
void brick::update() noexcept
{
    (void)0;
}

void brick::registerDiamondAmountCallback(const std::function<void(int)>& fnc)
{
    if (m_property == brick::DIAMOND && fnc) {
        m_diamond_amount_update_fnc = fnc;
        m_diamond_amount_update_fnc(1);
    }
}

void brick::registerPontUpdate(const std::function<void(int16_t)>& fnc) noexcept
{
    if (m_property == brick::DIAMOND || m_property == brick::BRICK) {
        m_point_update_fnc = fnc;
    }
}

void brick::registerParent(wall* parent) noexcept
{
    if (parent == nullptr)
        return;
    m_wall = parent;
    
    char char_id[10];
    auto result = std::to_chars(char_id, char_id + sizeof(char_id), m_wall->get_id());
    if (result.ec != std::errc()) {
        return;
    }

    subscribe("wall/destroyed");
}

void brick::on_message(const std::string& topic, Ientity* entity)
{
    if (topic == "wall/destroyed" && entity->get_id() == m_wall->get_id()) {
        m_wall = nullptr;
    }
}

void brick::draw(sf::RenderWindow& window) noexcept
{
    // Ask the window to draw the sprite for us
    draw_sprite(window);
}

brick::BrickProperty brick::getProperty() const noexcept
{
    return m_property;
}

std::set<brick*> brick::get_neighbors(sf::Vector2f range) const noexcept
{
    return std::set<brick*>();
}

void brick::hit(const int damage, const bool relate) noexcept
{
    m_hitCount += damage;
    bool destroyed = false;
    switch (m_property) {
    case BRICK:
    {
        hit_brick(destroyed, relate);
    } break;
    case DIAMOND:
    {
        hit_diamond(destroyed, relate);
    } break;
    case BOMB:
    {
        hit_bomb(destroyed, relate);
    } break;
    case SCALEUP:
    {
        hit_scaleup(destroyed, relate);
    } break;
    case CLONE:
    {
        hit_clone(destroyed, relate);
    } break;
    case NONE:
        [[fallthrough]];
    default:
        break;
    }
    if (destroyed) {
        destroy();
    }
}

void brick::hit_clone(bool& destroyed, const bool relate) const
{
    if (m_hitCount >= constants::cap_clone_hit) {
        destroyed = true;
    }
    if (!relate) {
        // SoundPlayer::getInstance()->playSoundEffect(SoundPlayer::SCALEUP_EFFECT);
    }
}

void brick::hit_scaleup(bool& destroyed, const bool relate) const
{
    if (m_hitCount >= constants::cap_scaleup_hit) {
        destroyed = true;
    }
    if (!relate) {
        // SoundPlayer::getInstance()->playSoundEffect(SoundPlayer::SCALEUP_EFFECT);
    }
}

void brick::hit_bomb(bool& destroyed, const bool relate) const
{
    if (m_hitCount >= constants::cap_bomb_hit) {
        destroyed = true;
    }
    if (!relate) {
        SoundPlayer::playSoundEffect(SoundPlayer::SoundEffect_t::BOMB_EXPLOSION);
    }
}

void brick::hit_diamond(bool& destroyed, const bool relate) const
{
    if (m_hitCount >= constants::cap_diamond_hit) {
        destroyed = true;
        if (m_diamond_amount_update_fnc) {
            m_diamond_amount_update_fnc(-1);
        }
        if (m_point_update_fnc) {
            m_point_update_fnc(int16_t(5));
        }
    }
    if (!relate) {
        SoundPlayer::playSoundEffect(SoundPlayer::SoundEffect_t::DIAMOND_DESTROY);
    }
}

void brick::hit_brick(bool& destroyed, const bool relate) const
{
    if (m_hitCount >= constants::cap_brick_hit) {
        destroyed = true;
        if (m_point_update_fnc) {
            m_point_update_fnc(int16_t(1));
        }
    }
    if (!relate) {
        SoundPlayer::playSoundEffect(SoundPlayer::SoundEffect_t::BRICK_BOUNCE);
    }
}

std::set<brick*> wall::get_brick_neighbors(const brick& a_brick, sf::Vector2f range) const noexcept
{
    const sf::Vector2f center_point{a_brick.x(), a_brick.y()};
    auto px_x = center_point.x - floorf((static_cast<float>(range.x) - 1.0F) / 2.0F) * a_brick.w();
    auto px_y = center_point.y - floorf((static_cast<float>(range.y) - 1.0F) / 2.0F) * a_brick.h();

    std::set<brick*> neighbors;
    for (int i = 0; i < range.y; ++i) {
        for (int j = 0; j < range.x; ++j) {
            const sf::Vector2f hit_point{px_x + static_cast<float>(i) * a_brick.w(),
                                            px_y + static_cast<float>(j) * a_brick.h()};

            try {
                auto* brick = this->m_data.at(hit_point).get();
                if (!brick->is_destroyed()) {
                    neighbors.insert(brick);
                }
            }
            catch (const std::out_of_range& e) {
                (void)e;
                continue;
            }
        }
    }
    return neighbors;
}

void wall::update()
{
    if (is_destroyed()) {
        return;
    }
    for (const auto& [position, a_brick] : this->m_data) {
        a_brick->update();
    }
}

void wall::draw(sf::RenderWindow& window) noexcept
{
    if (is_destroyed()) {
        return;
    }
    for (const auto& [position, a_brick] : this->m_data) {
        if (!a_brick->is_destroyed()) {
            a_brick->draw(window);
        }
    }
}

void wall::init([[maybe_unused]] float px_x, [[maybe_unused]] float px_y)
{
    (void)0;
}

void wall::load_from_file(std::filesystem::path file)
{
    this->clear();
    m_status.point = 0;

    std::error_code erc;
    if (std::filesystem::file_size(file, erc) == 0) {
        spdlog::error("Cannot open file {}: {}", file.string(), erc.message());
        return;
    }

    const rapidcsv::Document doc(file.string(), rapidcsv::LabelParams(-1, -1));

    const auto padding = (constants::window_width - constants::brick_width * doc.GetColumnCount()) / 2;
    for (decltype(doc.GetRowCount()) i = 0; i < doc.GetRowCount(); i++) {
        const auto rows = doc.GetRow<int>(i);
        for (decltype(rows.size()) j = 0; j < rows.size(); j++) {
            const auto px_x = static_cast<float>(padding + j * constants::brick_width);
            const auto px_y = static_cast<float>(i * constants::brick_height);
            const auto property = static_cast<brick::BrickProperty>(doc.GetCell<int>(j, i));
            auto a_brick = std::make_unique<brick>(px_x, px_y, property);
            a_brick->registerDiamondAmountCallback([this](int amount) mutable {
                this->m_status.live += amount;
            });
            a_brick->registerPontUpdate([this](short point) mutable {
                this->m_status.point += static_cast<uint16_t>(point);
            });
            a_brick->registerParent(this);
            this->m_data.emplace(e_location{px_x, px_y}, std::move(a_brick));
        }
    }
}

void wall::refresh()
{
    if (is_destroyed()) {
        return;
    }

    for (auto it = this->m_data.begin(); it != this->m_data.end();) {
        if (it->second->is_destroyed()) {
            it = this->m_data.erase(it);
        }
        else {
            ++it;
        }
    }

    if (this->m_status.live <= 0) {
        this->destroy();
    }
}

void wall::destroy() noexcept
{
    Ientity::destroy();
    publish("wall/destroyed", this);
}
