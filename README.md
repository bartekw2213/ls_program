# ls_program :page_facing_up:

Little copy of 'ls' Unix command written in C language. 

<br/>

### How to compile it? :hammer:

You will need a C compiler, I recommend using [GCC Gnu Compiler](https://gcc.gnu.org/). 
You need to compile it with `-lbsd` flag

```bash
gcc main.c -o main.out -lbsd
```

<br/>

### How to use it? :electric_plug:

```bash
./ls.out								# show files in the current directory
./ls.out <path>							# show files in specified directory
./ls.out -a <path>						# show all files including hidden one
./ls.out -l <path>						# show files with long listing format
./ls.out -R <path> 						# show files and list subdirectories recursively
./ls.out -n <path>						# like -l, but list numeric user and group IDs
./ls.out -G <path>						# in a long listing, don't print group names

```

<br/>

### Can I combine options together? :paperclip:

Yes, all formats shown below will work

```bash
./ls.out -alR
./ls.out -a -R -l -nG
```


