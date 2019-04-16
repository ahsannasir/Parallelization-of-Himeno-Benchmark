
#By Ahsan Nasir
FROM tudinfse/cds_server
RUN apt-get update && apt-get install -y \
    make \
    g++

#RUN apt-get install gcc && apt-get install make
RUN mkdir /usr/bin/mopp-2018-t0-harmonic-progression-sum
ADD ./mopp-2018-t0-harmonic-progression-sum /usr/bin/mopp-2018-t0-harmonic-progression-sum
WORKDIR /usr/bin/mopp-2018-t0-harmonic-progression-sum
COPY cds_server.json /etc/
RUN make 

#CMD ["./harmonic-progression-sum"]
