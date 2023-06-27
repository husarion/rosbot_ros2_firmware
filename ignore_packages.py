import os

try:
    os.makedirs('/home/runner/.platformio/packages/framework-mbed/features/')
except FileExistsError:
    print('Folder already exists')

with open('/home/runner/.platformio/packages/framework-mbed/features/.mbedignore', 'w') as f:
    f.write('cellular/*\n')
    f.write('netsocket/cellular/*\n')
    f.write('nanostack/*\n')