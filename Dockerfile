
#By Ahsan Nasir
FROM tudinfse/cds_server
RUN apt-get update && apt-get install -y \
    make \
    g++

#RUN apt-get install gcc && apt-get install make
RUN mkdir /usr/bin/mopp-2018-t3-himeno
ADD ./mopp-2018-t3-himeno /usr/bin/mopp-2018-t3-himeno


RUN mkdir /usr/bin/mopp-2017-t3-mandelbrot-set
ADD ./mopp-2017-t3-mandelbrot-set /usr/bin/mopp-2017-t3-mandelbrot-set

COPY cds_server.json /etc/

WORKDIR /usr/bin/mopp-2018-t3-himeno
RUN make 

# Running mandelbrot set

#RUN apt-get install gcc && apt-get install make

WORKDIR /usr/bin/mopp-2017-t3-mandelbrot-set
#COPY cds_server.json /etc/
RUN make 


#CMD ["./harmonic-progression-sum"]



# harmonic code

#By Ahsan Nasir
#FROM tudinfse/cds_server
#RUN apt-get update && apt-get install -y \
#    make \
#    g++

#RUN apt-get install gcc && apt-get install make
#RUN mkdir /usr/bin/mopp-2018-t0-harmonic-progression-sum
#ADD ./mopp-2018-t0-harmonic-progression-sum /usr/bin/mopp-2018-t0-harmonic-progression-sum
#WORKDIR /usr/bin/mopp-2018-t0-harmonic-progression-sum
#COPY cds_server.json /etc/
#RUN make 

#CMD ["./harmonic-progression-sum"]