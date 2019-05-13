#!/bin/bash
rm output_mp.txt
for i in 1 2 4 8 16
do
   for j in 3 5 7 9 11 13 15
    do
      for k  in 1 2 3
      do
        if [ $k -eq 1 ]
        then
          ./blur_effect_openmp 720Image.png 720Image_blur.png $j $i >> output_mp.txt
        elif [ $k -eq 2 ]
        then
          ./blur_effect_openmp 1080Image.png 1080Image_blur.png $j $i >> output_mp.txt
        else
          ./blur_effect_openmp 4kImage.png 4kImage_blur.png $j $i >> output_mp.txt
        fi
      done
      #echo "Threads: $i Blur-effect: $j"
    done
done
