FROM ubuntu

RUN apt-get update
RUN apt-get install git -y
RUN apt-get install g++ -y
RUN apt install cmake -y
RUN apt-get install libboost-all-dev -y
RUN apt-get install zlib1g-dev 

RUN mkdir HighloadCup2017
COPY . /HighloadCup2017/

RUN mkdir /HighloadCup2017/data

RUN cd HighloadCup2017/Build/ && bash build.sh

EXPOSE 80

CMD /HighloadCup2017/Build/Linux/Release/Server --ip 0.0.0.0 --port 80 --input /tmp/data/data.zip