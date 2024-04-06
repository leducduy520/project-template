#! /bin/bash

function askContinue()
{
    for i in {1..3}
    do
        echo "Continue the following jobs $i"
        sleep 1
    done
}

askContinue