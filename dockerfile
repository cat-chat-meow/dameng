FROM vdna-deps-env:latest

ENV MYSQL_ROOT_PASSWORD     123
ENV DB_PKGS                 /root/custom_sql

RUN apt-get install --force-yes -y \
    vim

COPY vimrc /root/.vimrc

COPY . /app
COPY entrypoint.sh /usr/local/bin/

RUN cp /app/init_mysql.sh /root/ && \
    bash /root/init_mysql.sh && \
    mkdir -p $DB_PKGS && mv /app/sql/* $DB_PKGS && \
    bash $DB_PKGS/db.sh && \
    sed -i "s/127.0.0.1/0.0.0.0/g" /etc/mysql/my.cnf

RUN echo "free"

# ENTRYPOINT ["entrypoint.sh"]

# CMD ["demo0"]
