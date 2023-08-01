set output "1.png"
set terminal pngcairo  transparent enhanced font "arial,10" fontscale 1.0 size 600, 400 
set auto x
set style data histogram
set style histogram cluster gap 20 
set style fill solid border -1
set boxwidth 0.9
plot "data.txt" using 2:xtic(1) with histogram

