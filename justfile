rebuild:
    #!/bin/bash
    # rm -rf .pio/
    # pio lib install
    pio run --target clean_microros  # Clean library
    pio lib install # Install dependencies
    pio run
    pio run --target upload