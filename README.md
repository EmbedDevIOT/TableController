# Table Controller ESP32 
## IoT контроллер управления механикой стола. на базе контроллера Espressif
<details>
<summary>Что умеет устройство?</summary>
  
  1. Управление линейными моторами (актуаторами) в количестве: 4шт
  
  2. Управление шаговым мотором NEMA 23
     
  3. Опрос и анализ 2х датчиков Холла
     
  4. Многофункциональная кнопка.
  </details>
  
 ![photo_2024-02-13_23-40-41](https://github.com/EmbedDevIOT/TableController/assets/99679510/4ada62db-3c6c-4a80-91cf-7c9277b295ff) 

## Устройство включает в себя
* MCU ESP-wroom-32
* Driver TB6600
* 4CH relay module
* 2 Hall sensors (NPN)
* DCDC Step Down 12 - 5V
* LiOn battery
* User Button
* Linear driver
* Step Driver Nema 23
* Optocuple logiq driver 3.3 to 5
