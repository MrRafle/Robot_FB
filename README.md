# Robot_FB
Это проект робота Alphabot на Raspberry Pi. Среда исполнения forte со всеми необходимыми кастомными блоками cкомпилирована для работы с RaspberryPi. Посредством блоков для MQTT и кастомного блока для работы с GPIO, робот исполняет команды, полученные с сервера, на котором запущена программа на основе OpenCV для ориентации и направления робота в пространстве по точкам-ориентирам трёх цветов (зеленый, желтый и белый). По достижении белой точки (конвейера), сервер подает команду загрузки на реле и по истечении времени
подает команду о завершении загрузки, затем робот движется по точкам в обратном порядке (желтый, зеленый), и, по достижении зеленой точки, сервер завершает работу.

Server.py - серверная часть

Robot_client - описание структуры программы в IDE. Блок SUBSCRIBE получает по MQTT от сервера команду в формате left, right, forward, stop. Эта команда передается на кастомный блок GPIO_controller, где она обрабатывается программой написанной нами раннее на cpp, инициируя заданные пины в определенном порядке в зависимости от поданной команды. В целях контроля все команды дублируются блоком OUT_ANY_CONSOLE.

<img width="986" height="416" alt="изображение" src="https://github.com/user-attachments/assets/818c4f28-414d-4d6a-807e-1b5137230372" />

gpiocontroller - кастомный функциональный блок gpio, написанный вручную на C++

<img width="383" height="158" alt="изображение" src="https://github.com/user-attachments/assets/459a725e-fbed-4ab8-b613-4ee9e93b7c29" />

setup_posix.sh - скрипт для сборки CMake под arm32

https://gitverse.ru/IT_Severstal/OpenSoftPLC_bin#user-content-%D0%BE-%D0%BF%D1%80%D0%BE%D0%B5%D0%BA%D1%82%D0%B5 - репозиторий с forte
