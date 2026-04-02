import subprocess
import time
import sys

# Resuming from Task 36
tasks = [
    ("Task 36", "Task 36. Untested SpatialHash insert: [JULES SUGGESTED] In test/collision/tSpatialHash.cpp, specifically test the insert method with AABBs that span negative coordinate quadrants (e.g., -100, -100 to 100, 100) to ensure bucket indexing is robust."),
    ("Task 37", "Task 37. Implement extension checking: [JULES SUGGESTED] In src/bytecode/generate.cpp:940, add a validation gate. If the compiler encounters a call to an external extension (e.g., SteamAPI), verify that it exists in the StandardLibrary map and throw a MiscError early if it doesn't."),
    ("Task 38", "Task 38. Confirm Transform Stack Size: [JULES SUGGESTED] Verify if k_transform_stack_size = 16 is enough for Undertale's rooms. Research if GameMaker Studio 1.4 has a higher limit and increase the constant to 32 if necessary to prevent stack overflow."),
    ("Task 39", "Task 39. Missing test file for SpatialHash: [JULES SUGGESTED] Create test/collision/tSpatialHash.cpp. Implement Catch2 sections for insert, remove, and foreach_leaf_in_aabb. This is critical for ensuring our new 3DS collision world is accurate."),
    ("Task 40", "Task 40. Store Reference to Prevent Struct Cleanup: [JULES SUGGESTED] In src/interpreter/execute.cpp:2335, manually increment the reference count of the active VariableStructData before the call opcode. This prevents the garbage collector from reaping the struct if the called script is the last thing holding a reference."),
    ("Task 41", "Task 41. Optimize Texture Page Lookup: [JULES SUGGESTED] In src/interpreter/display/TextureStore.cpp:666, replace the current linear scan for free texture space with a Binary Tree Bin Pack algorithm. This will reduce asset loading times for large data.win files by 50-70%."),
    ("Deep-Dive A", "Prompt A: The Global State & Refactor Audit. Jules, analyze the entire codebase for every instance of staticExecutor.m_frame.m_reflection. Find every point where it is still treated as a pointer (using ->) or where a null-check is performed. These are now compilation errors. Provide a list of every file that requires an address-of (&) fix or a syntax update to ensure the engine boots without crashing."),
    ("Deep-Dive B", "Prompt B: The Binary IFF Misalignment Analysis. Jules, perform a Deep-Dive into the DataWinLoader pointer logic. Analyze every chunk (OBJT, ROOM, CODE, TPAG). Determine if the ROOM instance list uses absolute file offsets or relative chunk offsets. Report every chunk that has more than 4 bytes of unknown header data."),
    ("Deep-Dive C", "Prompt C: The 3DS Linear RAM & Fragmentation Audit. Jules, do a Deep-Dive into the 3DS Heap. Calculate the memory footprint of Undertale's 63,977 strings and analyze the fragmentation risk when loading 12MB of TXTR data into linearAlloc memory. Provide a 'Memory Ceiling' report relative to the Old 3DS 32MB limit.")
]

repo = "Dxrmy/opengml-3ds"
spinner = ['|', '/', '-', '\\']

print(f"Jules Task Scheduler [Robust v2] started for {repo}...")

for name, desc in tasks:
    success = False
    spin_idx = 0
    while not success:
        sys.stdout.write(f"\r[{spinner[spin_idx % 4]}] Attempting {name}... ")
        sys.stdout.flush()
        spin_idx += 1
        
        cmd = f'jules remote new --repo {repo} --session "{desc}"'
        result = subprocess.run(["powershell", "-Command", cmd], capture_output=True, text=True)
        
        # Robust verification: check for confirmation string in stdout
        if result.returncode == 0 and "Session is created" in result.stdout:
            print(f"\r[✓] Pushed {name} successfully! Waiting 5s...")
            success = True
            time.sleep(5) 
        else:
            # Handle hidden failures or explicit errors
            reason = "API Reject" if result.returncode == 0 else "Network/Auth Error"
            sys.stdout.write(f"\r[X] {name} failed ({reason}). Retrying in 10s...")
            sys.stdout.flush()
            time.sleep(10)

print("\nAll tasks scheduled successfully.")
