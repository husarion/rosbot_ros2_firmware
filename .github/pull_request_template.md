# TODO: specify bump command and remove this description

Firmware release workflow is triggered when PR is closed, it bumps version, creates tag and release with firmware binary. To use it you have to specify bump type, available commands: `bump::patch`, `bump::minor` and `bump::major` (simply leave one of them in PR description and remove this description).