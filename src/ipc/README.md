# IPC System Documentation

This IPC (Inter-Process Communication) system provides a cross-platform solution for communication between a main process and multiple child processes using Boost.Interprocess.

## Features

- **Cross-platform**: Works on Windows, Linux, and macOS
- **Boost-based**: Uses Boost.Interprocess for reliable IPC
- **Message Queue Communication**: Uses Boost message queues for efficient message passing
- **Process Management**: Main process can spawn, monitor, and control multiple child processes
- **Command System**: Send commands (START, STOP, PAUSE, RESUME, etc.) to child processes
- **Status Monitoring**: Automatic heartbeat and status updates
- **Thread-safe**: All operations are thread-safe

## Architecture

### Components

1. **IPCMessage**: Message structure for IPC communication
2. **IPCManager**: Low-level IPC communication manager
3. **ChildProcess**: Child process implementation
4. **MainProcessManager**: Main process manager for spawning and controlling children

### Message Flow

```
Main Process (ID: 0)
    ↓ (spawns)
Child Process 1 (ID: 1) ←→ Message Queue 1
Child Process 2 (ID: 2) ←→ Message Queue 2
Child Process 3 (ID: 3) ←→ Message Queue 3
    ...
```

Each process has its own message queue named `{queue_name}_{process_id}`.

## Usage

### Building

The IPC system is automatically built when you build the project. Make sure `boost-interprocess` is in your `vcpkg.json` (already added).

### Main Process Example

```cpp
#include "MainProcessManager.hpp"

int main() {
    ipc::MainProcessManager manager("ipc_queue");
    
    if (!manager.initialize()) {
        return 1;
    }
    
    manager.startMonitoring();
    
    // Spawn child processes
    uint32_t pid1 = manager.spawnChildProcess("./ChildProcessExample");
    uint32_t pid2 = manager.spawnChildProcess("./ChildProcessExample");
    
    // Send commands
    manager.sendCommand(pid1, ipc::CommandType::START);
    manager.broadcastCommand(ipc::CommandType::START);
    
    // Check status
    auto status = manager.getChildStatus(pid1);
    
    // Cleanup
    manager.cleanup();
    return 0;
}
```

### Child Process Example

```cpp
#include "ChildProcess.hpp"

int main(int argc, char* argv[]) {
    uint32_t process_id = std::atoi(argv[1]);
    
    ipc::ChildProcess child(process_id, "ipc_queue");
    
    if (!child.initialize()) {
        return 1;
    }
    
    child.run();  // Blocks until stopped
    return 0;
}
```

### Custom Child Process

You can derive from `ChildProcess` to implement custom behavior:

```cpp
class MyChildProcess : public ipc::ChildProcess {
public:
    MyChildProcess(uint32_t id) : ChildProcess(id, "ipc_queue") {}
    
protected:
    void doWork() override {
        // Your custom work here
    }
    
    void onInitialize() override {
        // Custom initialization
    }
    
    void onCleanup() override {
        // Custom cleanup
    }
};
```

## Message Types

- **COMMAND**: Command from main to child
- **RESPONSE**: Response from child to main
- **STATUS**: Status update from child
- **HEARTBEAT**: Periodic heartbeat message
- **TERMINATE**: Termination request

## Command Types

- **START**: Start the child process
- **STOP**: Stop the child process
- **PAUSE**: Pause the child process
- **RESUME**: Resume a paused process
- **GET_STATUS**: Request status update
- **CUSTOM**: Custom command (handle in derived classes)

## Process Status

- **IDLE**: Process is idle
- **RUNNING**: Process is running
- **PAUSED**: Process is paused
- **STOPPED**: Process is stopped
- **ERROR**: Process is in error state

## Platform Support

- **Windows**: Uses `CreateProcess` for spawning
- **Linux/Unix**: Uses `fork()` and `execv()` for spawning
- **macOS**: Uses `fork()` and `execv()` for spawning

## Thread Safety

All IPC operations are thread-safe. Multiple threads can safely:
- Send messages
- Receive messages
- Query process status
- Spawn/control processes

## Error Handling

The system handles errors gracefully:
- Failed message sends return `false`
- Dead processes are detected via heartbeat timeout (10 seconds)
- Queue creation failures are handled
- Process spawn failures return 0

## Example Executables

Two example executables are provided:
- `MainProcessExample`: Demonstrates main process usage
- `ChildProcessExample`: Demonstrates child process usage

Run them like this:

```bash
# Terminal 1: Run main process
./MainProcessExample ./ChildProcessExample

# The main process will spawn child processes automatically
```

## Notes

- Message queues are automatically cleaned up on process exit
- Each process should have a unique process ID
- The main process always has ID 0
- Child processes receive their ID as a command-line argument
- Heartbeats are sent every 5 seconds
- Dead process detection timeout is 10 seconds

## Dependencies

- Boost.Interprocess
- C++20 standard
- Thread support
