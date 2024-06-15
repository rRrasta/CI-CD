#!/bin/bash

commands=(
    "diff <(./s21_cat cat_test.txt) <(cat cat_test.txt)"
    "diff <(./s21_cat -e cat_test.txt) <(cat -e cat_test.txt)"
    "diff <(./s21_cat -v cat_test.txt) <(cat -v cat_test.txt)"
    "diff <(./s21_cat -t cat_test.txt) <(cat -t cat_test.txt)"
    "diff <(./s21_cat -s cat_test.txt) <(cat -s cat_test.txt)"
    "diff <(./s21_cat --squeeze-blank cat_test.txt) <(cat -s cat_test.txt)"
    "diff <(./s21_cat -n cat_test.txt) <(cat -n cat_test.txt)"
    "diff <(./s21_cat --number cat_test.txt) <(cat -n cat_test.txt)"
    "diff <(./s21_cat -b cat_test.txt) <(cat -b cat_test.txt)"
    "diff <(./s21_cat --number-nonblank cat_test.txt) <(cat -b cat_test.txt)"
    "diff <(./s21_cat -etsnvb cat_test.txt) <(cat -etsnvb cat_test.txt)"
)

error_flag=0

for cmd in "${commands[@]}"; do
    eval "$cmd"
    if [ $? -ne 0 ]; then
        echo "FAIL"
        error_flag=1
        break
    fi

done

if [ $error_flag -eq 0 ]; then
    echo "SUCCESS"
fi
