for i in `seq 1 ${2}`
do
	./crossover adam.evl eva.evl ${1}-${i}.evl
	echo ${1}-${i}.evl
done
