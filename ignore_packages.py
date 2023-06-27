with open('/home/husarion/.platformio/packages/framework-mbed/features/.mbedignore', 'w') as f:
    f.write('cellular/*\n')
    f.write('netsocket/cellular/*\n')
    f.write('nanostack/*\n')