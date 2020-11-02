FROM python:latest

# install google chrome
RUN wget -q -O - https://dl-ssl.google.com/linux/linux_signing_key.pub | apt-key add -
RUN sh -c 'echo "deb [arch=amd64] http://dl.google.com/linux/chrome/deb/ stable main" >> /etc/apt/sources.list.d/google-chrome.list'
RUN apt-get -y update
RUN apt-get install -y google-chrome-stable

# install Chinese fonts
RUN wget http://fbi-fonts.oss-cn-shanghai.aliyuncs.com/msyh.ttf -O /home/msyh.ttf 
RUN mkdir -p /usr/share/fonts/chinese/TrueType/ 
RUN cp /home/msyh.ttf /usr/share/fonts/chinese/TrueType/ 
RUN fc-cache -fv


# install chromedriver
RUN apt-get install -yqq unzip
RUN wget -O /tmp/chromedriver.zip http://chromedriver.storage.googleapis.com/`curl -sS chromedriver.storage.googleapis.com/LATEST_RELEASE`/chromedriver_linux64.zip
RUN unzip /tmp/chromedriver.zip chromedriver -d /usr/local/bin/

# set display port to avoid crash
ENV DISPLAY=:99

# upgrade pip
RUN pip install --upgrade pip

# install pymsql
RUN pip install pymysql

# install selenium
RUN pip install selenium

# install pyecharts
RUN pip install pyecharts

# install snapshot-selenium
RUN pip install snapshot-selenium

# add newuser as chrome cannot run by root account
RUN groupadd --gid 5000 newuser \
    && useradd --uid 5000 \
        --gid 5000 --shell /bin/bash newuser

USER newuser

WORKDIR /home
