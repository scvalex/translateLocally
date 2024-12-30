default:
    just --choose

# Prepare repo for editing with helix
prepare:
    mkdir -p build/
    cd build/ && cmake ..
    cd build/ && cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cp build/compile_commands.json .

