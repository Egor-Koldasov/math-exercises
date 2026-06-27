#!/bin/bash

getRandomNumber() {
  RAND_MIN=${1:-1};
  RAND_MAX=${2:-1000};


  bc -l <<EOF
scale=20;
random_num = irand($RAND_MIN+$RAND_MAX+1);
random_norm=random_num/($RAND_MIN+$RAND_MAX);
random_inter=r(random_norm*($RAND_MAX-$RAND_MIN)+$RAND_MIN, 0);
random_inter;
EOF
}

random_number=$(getRandomNumber)

echo $random_number
