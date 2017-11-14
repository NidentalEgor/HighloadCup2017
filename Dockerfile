FROM ubuntu

RUN apt-get update
RUN apt-get install git -y
RUN apt-get install g++ -y
RUN apt install cmake -y
RUN apt-get install unzip -y
RUN apt-get install libboost-all-dev -y
RUN apt-get install unzip -y

RUN mkdir HighloadCup2017
COPY . /HighloadCup2017/
# RUN mkdir DataZip
# COPY /home/egor/Repositories/highload_data_zip/data.zip /Data/data.zip
# RUN mkdir DataUnzip
# RUN unzip /Data/data.zip -d /DataUnzip
# RUN cd HighloadCup2017 && ls
# RUN cd HighloadCup2017/Build && ls
# RUN cd HighloadCup2017/Submodules && ls
# RUN cd HighloadCup2017/HttpServer && ls
# RUN cd HighloadCup2017/DataStorage && ls
# RUN cd HighloadCup2017/Utils && ls

RUN mkdir /HighloadCup2017/data
RUN unzip /HighloadCup2017/data.zip -d /HighloadCup2017/data

RUN cd HighloadCup2017/Build/ && bash build.sh

EXPOSE 80

CMD /HighloadCup2017/Build/Linux/Release/Server