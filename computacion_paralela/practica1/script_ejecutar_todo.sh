#!/bin/bash
rm output.txt
for i in 1 2 4 8 16
do
   for j in 3 5 7 9 11 13 15
    do
      for k  in 1 2 3
      do
        if [ $k -eq 1 ]
        then
          ./test_pnglib 720Image.png 720Image_blur.png $j $i >> output.txt
        elif [ $k -eq 2 ]
        then
          ./test_pnglib 1080Image.png 1080Image_blur.png $j $i >> output.txt
        else
          ./test_pnglib 4kImage.png 4kImage_blur.png $j $i >> output.txt
        fi
      done
      #echo "Threads: $i Blur-effect: $j"
    done
done
