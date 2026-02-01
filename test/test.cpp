// Third-party library includes
#include <gtest/gtest.h>
#include <SFML/Graphics.hpp>

// Standard library includes
#include <filesystem>
#include <memory>
#include <vector>
#ifdef _WIN32
#include <Windows.h>
#else
#include <linux/limits.h>
#include <unistd.h>
#endif

// Project includes
#include <background.hpp>
#include <ball.hpp>
#include <brick.hpp>
#include <constants.hpp>
#include <countingtext.hpp>
#include <entity.hpp>
#include <entitymanager.hpp>
#include <helper.hpp>
#include <interactions.hpp>
#include <paddle.hpp>
#include <resource_integrity.hpp>
#include <soundplayer.hpp>
#include <threadpool.hpp>
#include <timer.hpp>

std::filesystem::path GetExecutablePath()
{
#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    return std::filesystem::path(path).parent_path();
#else
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    return std::filesystem::path(std::string(path, (count > 0) ? count : 0)).parent_path();
#endif
}

#define SELF_PATH GetExecutablePath()

constants::resouces_path = SELF_PATH / "resources";

// Test helper classes
class TestEntity : public Ientity
{
public:
    void update() override {}
    void draw(sf::RenderWindow&) override {}
    void init(float, float) override {}
};

class TestStaticEntity : public static_entity
{
public:

    struct message_data
    {
        size_t entity_id;
        std::string topic;
    };

    void on_message(const std::string& topic, Ientity* entity) override {
        std::cout << "TestStaticEntity <" << get_id() << "> received message" << std::endl;
        std::cout << "Topic: " << topic << std::endl;
        std::cout << "Entity ID: " << entity->get_id() << std::endl;
        m_received_messages.push_back({entity->get_id(), topic});
    }
    size_t get_received_messages_size() const noexcept
    {
        return m_received_messages.size();
    }
    const message_data& get_received_message(size_t index) const noexcept
    {
        if (index >= m_received_messages.size()) {
            throw std::out_of_range("Index out of range");
        }
        return m_received_messages[index];
    }
    void do_test_subscribe() noexcept
    {
        subscribe("test");
    }
    void do_test_publish() noexcept
    {
        publish("test", this);
    }
private:
    std::vector<message_data> m_received_messages;
};

class TestMovingEntity : public moving_entity
{
public:
    void move_up() noexcept override {}
    void move_down() noexcept override {}
    void move_left() noexcept override {}
    void move_right() noexcept override {}
};

// ============================================================================
// Ientity Tests
// ============================================================================

TEST(EntityTest, Ientity_Construction)
{
    TestEntity entity;
    EXPECT_FALSE(entity.is_destroyed());
    EXPECT_EQ(entity.get_id(), 0);
}

TEST(EntityTest, Ientity_Destroy)
{
    TestEntity entity;
    EXPECT_FALSE(entity.is_destroyed());
    entity.destroy();
    EXPECT_TRUE(entity.is_destroyed());
}

TEST(EntityTest, Ientity_GetTypeName)
{
    std::string type_name = Ientity::get_type_name<TestEntity>();
    EXPECT_FALSE(type_name.empty());
    
    std::string int_name = Ientity::get_type_name<int>();
    EXPECT_FALSE(int_name.empty());
}

TEST(EntityTest, Ientity_CopyDeleted)
{
    // Verify that copy constructor and assignment are deleted
    TestEntity entity1;
    // These should not compile, but we can test that the class is not copyable
    static_assert(!std::is_copy_constructible_v<TestEntity>);
    static_assert(!std::is_copy_assignable_v<TestEntity>);
}

// ============================================================================
// static_entity Tests
// ============================================================================

TEST(EntityTest, StaticEntity_Construction)
{
    TestStaticEntity entity;
    EXPECT_FALSE(entity.is_destroyed());
}

TEST(EntityTest, StaticEntity_Position)
{
    TestStaticEntity entity;
    
    // Initial position should be (0, 0)
    EXPECT_FLOAT_EQ(entity.x(), 0.0f);
    EXPECT_FLOAT_EQ(entity.y(), 0.0f);
    
    // Set position
    entity.set_position({100.0f, 200.0f});
    EXPECT_FLOAT_EQ(entity.x(), 100.0f);
    EXPECT_FLOAT_EQ(entity.y(), 200.0f);
}

TEST(EntityTest, StaticEntity_Move)
{
    TestStaticEntity entity;
    entity.set_position({10.0f, 20.0f});
    
    entity.move({5.0f, -10.0f});
    EXPECT_FLOAT_EQ(entity.x(), 15.0f);
    EXPECT_FLOAT_EQ(entity.y(), 10.0f);
    
    entity.move({-3.0f, 7.0f});
    EXPECT_FLOAT_EQ(entity.x(), 12.0f);
    EXPECT_FLOAT_EQ(entity.y(), 17.0f);
}

TEST(EntityTest, StaticEntity_Scale)
{
    TestStaticEntity entity;
    
    // Set scale
    entity.set_scale(2.0f, 3.0f);
    // Scale doesn't affect position, but we can verify it was set
    // (actual scale verification would require texture to be set)
}

TEST(EntityTest, StaticEntity_Bounds)
{
    TestStaticEntity entity;
    entity.set_position({50.0f, 100.0f});
    
    // Test bounds methods (they depend on sprite size, which may be 0 initially)
    float left = entity.left();
    float top = entity.top();
    float right = entity.right();
    float bottom = entity.bottom();
    
    // Right should be left + width
    EXPECT_FLOAT_EQ(right, left + entity.w());
    // Bottom should be top + height
    EXPECT_FLOAT_EQ(bottom, top + entity.h());
}

TEST(EntityTest, StaticEntity_Dimensions)
{
    TestStaticEntity entity;
    
    // Dimensions depend on sprite/texture, may be 0 initially
    float w = entity.w();
    float h = entity.h();
    
    EXPECT_GE(w, 0.0f);
    EXPECT_GE(h, 0.0f);
}

TEST(EntityTest, StaticEntity_ResourceIntegrity)
{
    EXPECT_NO_THROW(TestStaticEntity::check_resource_integrity(constants::resouces_path / "ball.png"));
    EXPECT_THROW(TestStaticEntity::check_resource_integrity(constants::resouces_path / "ball_nonexistent.png"), std::logic_error);
}

// ============================================================================
// moving_entity Tests
// ============================================================================

TEST(EntityTest, MovingEntity_Construction)
{
    TestMovingEntity entity;
    EXPECT_FALSE(entity.is_destroyed());
    
    // Initial velocity should be (0, 0)
    sf::Vector2f vel = entity.get_velocity();
    EXPECT_FLOAT_EQ(vel.x, 0.0f);
    EXPECT_FLOAT_EQ(vel.y, 0.0f);
}

TEST(EntityTest, MovingEntity_SetVelocity)
{
    TestMovingEntity entity;
    
    entity.set_velocity({5.0f, -10.0f});
    sf::Vector2f vel = entity.get_velocity();
    EXPECT_FLOAT_EQ(vel.x, 5.0f);
    EXPECT_FLOAT_EQ(vel.y, -10.0f);
    
    entity.set_velocity({0.0f, 0.0f});
    vel = entity.get_velocity();
    EXPECT_FLOAT_EQ(vel.x, 0.0f);
    EXPECT_FLOAT_EQ(vel.y, 0.0f);
}

TEST(EntityTest, MovingEntity_VelocityUpdate)
{
    TestMovingEntity entity;
    
    entity.set_velocity({3.5f, 7.2f});
    sf::Vector2f vel1 = entity.get_velocity();
    
    entity.set_velocity({-1.0f, 2.5f});
    sf::Vector2f vel2 = entity.get_velocity();
    
    EXPECT_FLOAT_EQ(vel2.x, -1.0f);
    EXPECT_FLOAT_EQ(vel2.y, 2.5f);
    EXPECT_NE(vel1.x, vel2.x);
    EXPECT_NE(vel1.y, vel2.y);
}

TEST(EntityTest, MovingEntity_InheritsFromStaticEntity)
{
    TestMovingEntity entity;
    
    // Should inherit position methods
    entity.set_position({100.0f, 200.0f});
    EXPECT_FLOAT_EQ(entity.x(), 100.0f);
    EXPECT_FLOAT_EQ(entity.y(), 200.0f);
    
    // Should inherit move method
    entity.move({10.0f, 20.0f});
    EXPECT_FLOAT_EQ(entity.x(), 110.0f);
    EXPECT_FLOAT_EQ(entity.y(), 220.0f);
}

TEST(TestEntityManager, CreateEntity)
{
    auto m_entity_manager = std::make_shared<entity_manager>();
    auto& entity1 = m_entity_manager->create<TestStaticEntity>();
    EXPECT_EQ(entity1.get_id(), 1); // First entity should have ID 1
    auto& entity2 = m_entity_manager->create<TestStaticEntity>();
    EXPECT_EQ(entity2.get_id(), 2); // Second entity should have ID 2
    auto& entity3 = m_entity_manager->create<TestStaticEntity>();
    EXPECT_EQ(entity3.get_id(), 3); // Third entity should have ID 3

    entity2.destroy();
    m_entity_manager->refresh();
    EXPECT_EQ(m_entity_manager->get_all<TestStaticEntity>().size(), 2); // Two entities should be left

    auto& entity4 = m_entity_manager->create<TestStaticEntity>();
    EXPECT_EQ(entity4.get_id(), 2); // Fourth entity should have ID 2 because the second entity was destroyed and the ID was recycled

    m_entity_manager->clear();
    EXPECT_EQ(m_entity_manager->get_all<TestStaticEntity>().size(), 0); // No entities should be left
}

TEST(TestEntityManager, PublishAndSubscribe)
{
    auto m_entity_manager = std::make_shared<entity_manager>();
    auto& entity1 = m_entity_manager->create<TestStaticEntity>();
    auto& entity2 = m_entity_manager->create<TestStaticEntity>();
    entity1.do_test_subscribe();
    entity2.do_test_subscribe();
    entity1.do_test_publish();
    EXPECT_EQ(entity2.get_received_messages_size(), 1);
    EXPECT_EQ(entity2.get_received_message(0).entity_id, entity1.get_id());
    EXPECT_EQ(entity2.get_received_message(0).topic, "test");
    EXPECT_EQ(entity1.get_received_messages_size(), 1);
    EXPECT_EQ(entity1.get_received_message(0).entity_id, entity1.get_id());
    EXPECT_EQ(entity1.get_received_message(0).topic, "test");
}

TEST(TestSetup, GoogleTestWorks)
{
    EXPECT_TRUE(true);
    EXPECT_EQ(1 + 1, 2);
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    // Only parse YOUR arguments after InitGoogleTest
    // argc/argv are cleaned of --gtest_* flags

    return RUN_ALL_TESTS();
}