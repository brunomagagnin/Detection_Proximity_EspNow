# PROJETO DE SEGURANÇA DE PEDESTRES EM MOVIMENTAÇÃO DE CARGAS

Projeto baseado em hardware M5Core2 e M5ATOM

![alt text](https://github.com/brunomagagnin/Detection_Proximity_EspNow/blob/main/Imagem1.png)

## DESCRIÇÃO GERAL
Cada máquina possuirá um dispositivo M5Core e 2 a 4 sensores externos M5Atom, e os pedestres um dispositivo M5Core. Quando o pedestre se aproximar da máquina o dispositivo entrará em modo de alerta, vibrando e emitindo um sinal sonoro até que o pedestre se afaste da máquina. 
Caso o pedestre deseje se aproximar da máquina, deverá pressionar o botão "Requisição" na tela do dispositivo, o operador da máquina por sua vez receberá um alerta no dispositivo podendo aceitar ou não a aproximação do pedestre. Caso o operador da máquina aceite a aproximação do pedestre, o dispositivo do pedestre receberá o "aceite" e aparecerá um ícone de alerta nos dispositivos da máquina e do pedestre. 

![alt text](https://github.com/brunomagagnin/Detection_Proximity_EspNow/blob/main/Imagem2.png)
![alt text](https://github.com/brunomagagnin/Detection_Proximity_EspNow/blob/main/Imagem3.png)
![alt text](https://github.com/brunomagagnin/Detection_Proximity_EspNow/blob/main/Imagem4.png)
![alt text](https://github.com/brunomagagnin/Detection_Proximity_EspNow/blob/main/Imagem5.png)

### 📋 Hardaware e Software necessário
* 2 M5STACK CORE2    https://docs.m5stack.com/en/core/core2
* Watch Kit          https://shop.m5stack.com/products/watch-v1-1-dev-kit-excluding-core?variant=43918752907521
* 2 À 4 M5ATOM Lite  https://docs.m5stack.com/en/core/atom_lite
* VSCODE
* Platformio

### Especificações do projeto
- Para até duas máquinas de movimentação de cargas
- A quantidade de pedestres que utilizam o sistema não há restrição
- até 4 sensores externos

### 🛠️ Construído com
* VSCode + Platformio
* API gráfica: LVGL 8;

## 🔧 Instalação e configuração
* No projeto há 3 pastas: machine, person e sender. Machine é referente ao M5Core que ficará em cada máquina, person é referente ao M5Core que ficará com os pedestres e sender é referente aos sensores que poderão ser instalados externamente nas máquinas.
* O sistema pode ser utilizado de duas formas: 
  - Com um M5Core em cada máquina e um M5Core para cada pedestre. Nesse modo existe problema de funcionamento devido a barreiras físicas
  - Com um M5Core em cada máquina, 2 à 4 sensores externos nas máquinas e um M5Core para cada pedestre. Maior confiabilidade do sistema contra barreiras físicas do próprio equipamento.
* Os sensores externos (M5Atom) em caso de uso, se fazem necessários pois barreiras físicas do próprio equipamento atrapalham a força do sinal.
* Em locais onde há incidencia de inúmeros roteadores wifi, pode haver perda de sinal. Para isso é necessário fazer um estudo e adequar o sistema para utilizar um canal que não entre em conflito com os demais sinais existentes. 
* Config.h arquivo de configurações gerais como: canal wifi a ser utilizado, força do sinal, tempos etc.
* Mac_Address.h arquivo de configuração dos endereço de todos os hardwares utilizados.


## ✒️ Autor
* **BRUNO MAGAGNIN PEREIRA**
