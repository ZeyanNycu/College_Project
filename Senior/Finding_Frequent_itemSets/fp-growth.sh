Dataset=A_1000_500.data
python3 fp-growth.py -f $Dataset \
                   -s 0.002  \
                   -d ./statics

python3 fp-growth.py -f $Dataset \
                   -s 0.005  \
                   -d ./statics

python3 fp-growth.py -f $Dataset \
                   -s 0.01  \
                   -d ./statics
        
Dataset=B_100000_600.data
python3 fp-growth.py -f $Dataset \
                   -s 0.0015  \
                   -d ./statics

python3 fp-growth.py -f $Dataset \
                   -s 0.002  \
                   -d ./statics

python3 fp-growth.py -f $Dataset \
                   -s 0.005  \
                   -d ./statics

Dataset=C_1000000_600.data
python3 fp-growth.py -f $Dataset \
                   -s 0.01  \
                   -d ./statics

python3 fp-growth.py -f $Dataset \
                   -s 0.02  \
                   -d ./statics

python3 fp-growth.py -f $Dataset \
                   -s 0.03  \
                   -d ./statics