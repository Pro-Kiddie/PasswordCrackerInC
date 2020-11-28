# Password Cracker in C

## Rainbow Table Generator

Generates a rainbow table in the format of `/etc/shadow` for hashing algorithms of `$1$` and `$6$` with empty salt value, given a wordlist.

`man 5 crypt` to check all available hashing algorithms.

```bash
# Compile
gcc gen_rainbow.c -lcrypt -o gen_rainbow

./gen_rainbow small_wordlist.txt 1 12
# Usage : ./gen_rainbow <wordlist> <min> <max>
#        <wordlist> : A file path/name in which contains the password dictionary.
#        <min> : An integer value greater than 1.
#                This value represents the minimum length of the password to be processed.
#        <max> : An integer value greater than or equal to <min>.
#                <max> represents the maximum length of the password to be processed.

```

## Password Cracker

Loads the rainbow table file generated from above into a linked list and try to crack each entry's hash in `shadow` file supplied.

Validates the entries in `shadow` file supplied use either `$1$` of `$6$` hashing algorithm and empty salt value.

```bash
# Compile
gcc password_cracker.c -o password_cracker

./password_cracker shadow.txt mytab2411.txt
# Usage : ./password_cracker <shadow file> <rainbow table>
#        <shadow file> : A file path/name in which contains the hashed password for every user.
#        <rainbow table> : A file path/name in which contains the plain password and their respective hashed values.

```