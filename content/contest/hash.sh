# Hashes a file, ignoring all whitespace and comments. Use for
# verifying that code was correctly typed.
cpp -dD -P -fpreprocessed | tr -d '[:space:]'| md5sum |cut -c-6
# eg:
# cat sus.cpp | cpp -dD -P -fpreprocessed | tr -d '[:space:]'| md5sum |cut -c-6 