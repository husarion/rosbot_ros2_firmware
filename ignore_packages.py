import os
user_name = os.getenv('USER')
try:
    os.makedirs(f'/home/{user_name}/.platformio/packages/framework-mbed/features/')
except FileExistsError:
    print('Folder already exists')

with open(f'/home/{user_name}/.platformio/packages/framework-mbed/features/.mbedignore', 'w') as f:
    f.write('cellular/*\n')
    f.write('netsocket/cellular/*\n')
    f.write('nanostack/*\n')