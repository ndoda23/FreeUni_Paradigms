

## Environment Setup


### Ubuntu
```sh
sudo apt-get install valgrind
```
### Arch
```sh
yay -S valgrind
```

## Compilation
```sh
make
```

## Run a Single Test
```sh
./rsg data/bionic.g
```

## Memory check
```sh
./rsgChecker32 ./rsg data/bionic.g
./rsgChecker64 ./rsg data/bionic.g
```

## Run All Tests
```sh
make test_all
``` 
