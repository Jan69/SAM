#!/bin/sh
clear
SAM(){ ../sam "$@";}
sleep(){ true;}
#overcomplicated for sure, lol
#beware, unix "[]" and "test" tools assume numbers starting with 0 are octal or something
number="$(strings /dev/urandom|grep "[0-9]"|head -n 2|sed "s/[^0-9]//g"|tr -d "[:space:]"|grep -o "[1-9]\?[1-9]")"
sleep 1
echo "Sam's NUMBER GUESSING GAME!"
SAM "Sam's NUMBER GUESSING GAME!"
sleep 1
name="Insert Username Here"
#true <<"#EOF" #uncomment to skip the welcome phrase
echo
sleep 1
echo
sleep 1
echo
sleep 1
echo "It's easy to play!"
SAM "It's easy to play!"
sleep 1
echo
sleep 1
echo "The computer generates a number between 1 and 100, and you try to guess it!"
SAM "The computer generates a number between 1 and one hundred, and you try to guess it!"
sleep 1
echo
sleep 1
echo
sleep 1
echo
echo "To begin the game, type your name and press Enter":
SAM "To begin the game, type your name and press Enter":
read name
clear
echo "Well $name Now GUESS THE NUMBER!"
SAM "Well"
SAM "$name"
SAM "Now GUESS THE NUMBER!"
#EOF
echo
echo
while read guess;do
  if test "$guess" -lt "$number";then
    clear
    echo "Your guess is lower than the number!"
    SAM "Your guess is lower than the number!"
    echo
    echo
  elif test "$guess" -gt "$number";then
    clear
    echo "Your guess is higher than the number!"
    SAM "Your guess is higher than the number!"
    echo
    echo
  elif test "$guess" -eq "$number";then
    clear
    echo
    SAM -speed 55 -pitch 92 -throat 128 -mouth 128 "Ha ha ha ha ha ha ha? Ha ha ha ha ha ha ha!"
    echo "Congratulations ${name}! You guessed the number correctly!"
    SAM "Congratulations"
    SAM "${name}!"
    SAM "You guessed the number correctly!"
    echo
    echo
    echo "The number was $number"
    SAM "The number was"
    SAM "$number"
    echo "Thanks for playing!"
    SAM "Thanks for playing?"
    SAM "$name!"
    SAM "Thanks for playing."
    sleep 2
    exit 0
  fi
done
