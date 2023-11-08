import os
user_name = os.getenv('USER')
if user_name == None:
    print("There is no global variable 'USER'!!!")
    exit(-1)
try:
    os.makedirs(f'/home/{user_name}/.platformio/packages/framework-mbed/features/')
except FileExistsError:
    print('Folder already exists')

with open(f'/home/{user_name}/.platformio/packages/framework-mbed/features/.mbedignore', 'w') as f:
    f.write('cellular/*\n')
    f.write('netsocket/cellular/*\n')
    f.write('nanostack/*\n')
