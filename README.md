# Fluxo-de-Pessoas

O projeto foi feito para o controle de pessoas de um local, ou seja podemos ver o horário de pico de um estabelecimento, controlar o limite de pessoas em uma sala.

Esse é o fluxo de pessoas:

![](https://github.com/JoaoVictorT/Fluxo-de-Pessoas/blob/main/FluxoDePessoas.jpeg)


Ele tem dois buracos onde vão dois sensores LDR, e vão receber luminosidade de dois lasers, que na instalação ficariam do outro lado da porta.

Esses são os dois lasers:

![](https://github.com/JoaoVictorT/Fluxo-de-Pessoas/blob/main/Laser.jpeg)

O funcionamento dele seria, dois lasers estão iluminando os ldrs, ao momento que a pessoa passar vai interromper o laser, e o ldr vai ter uma variação na tensão, e assim descobrimos.
Os dois lasers servem para identificarmos se a pessoa esta saindo ou entrando, e depois de identificar, o outro laser atuará como um verificador para ver se realmente, a pessoa saiu ou entrou.


Para podermos enviar informações criamos uma máquina virtual na azure, em unbuntu, nele instalamos o telefrag, mosquitto, influxdb e o grafana.

Usamos o telegraf para fazer a conexão entre o mosquitto, influxdb e o grafana.

O mosquitto foi utlizado para criar um broker, e também criar um subscriber, onde ele se inscreveu no topico criado no firmware, e o firmware foi o publisher, então
todos as informações passadas para o mosquitto, eram passadas para o influxdb que é um banco de dados, que por fim o grafana, pegava as informações do banco de dados, para fazer os gráficos.

![](https://github.com/JoaoVictorT/Fluxo-de-Pessoas/blob/main/telegraf.png)


