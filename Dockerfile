FROM centos:latest
RUN mkdir -p /app/
COPY runtime/* /app/
WORKDIR /app/
ENV  LD_LIBRARY_PATH=/app/:$LD_LIBRARY_PATH
CMD ["./cmfs"]

