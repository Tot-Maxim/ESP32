# **Проект: Система мониторинга и управления через MQTT на базе ESP32**

## **Описание проекта**

Данный репозиторий содержит инструкции и конфигурации для развертывания **MQTT-брокера Eclipse Mosquitto** на **Windows** и его интеграции с **ESP32 (DevKit с CP2102)** для передачи данных, а также визуализации и управления через **Node-RED**.

### **Ключевые компоненты системы**

1. **ESP32-CP2102 (Dev Module)**
    
    - Микроконтроллер с Wi-Fi/Bluetooth
        
    - Подключение датчиков (например, освещенности BH1750)
        
    - Отправка данных в MQTT-брокер
        
    - Получение управляющих команд (например, для светодиодов или дисплея)
        
2. **Eclipse Mosquitto (MQTT-брокер)**
    
    - Развертывание на **Windows**
        
    - Централизованный обмен сообщениями между устройствами
        
    - Поддержка топиков для публикации и подписки
        
3. **Node-RED**
    
    - Визуальное программирование логики обработки данных
        
    - Дашборд для мониторинга показаний датчиков
        
    - Управление устройствами через MQTT-команды
        

---

## **Функционал системы**

✅ **Сбор данных** (например, с датчика освещенности BH1750)  
✅ **Передача по MQTT** (публикация в топик `sensors/light`)  
✅ **Визуализация в Node-RED** (графики, индикаторы, текстовые блоки)  
✅ **Обратная связь** (управление светодиодом/дисплеем на ESP32)

### **Пример работы**

1. **ESP32** отправляет данные освещенности → `sensors/light`
    
2. **Node-RED** обрабатывает данные и выводит их на дашборд
    
3. При достижении пороговых значений → отправка команд (`light/control`, `display/control`)
    
4. **ESP32** получает команды и выполняет действия (включение светодиода, смена режима дисплея)
    

---

## **Стек технологий**

- **Аппаратная часть:**
    
    - ESP32 (DevKit с чипом CP2102 для UART-связи)
        
    - Датчики (BH1750, DHT22 и др.)
        
- **Программное обеспечение:**
    
    - Eclipse Mosquitto (MQTT-брокер)
        
    - Node-RED (логика + веб-интерфейс)
        
    - Arduino IDE / PlatformIO (прошивка ESP32)

---

## **Как развернуть?**

1. Установите **Mosquitto** на Windows
    
2. Настройте **ESP32** для отправки данных в MQTT
    
3. Разверните **Node-RED** и импортируйте готовый дашборд
    
4. Подключите устройства и проверьте работу системы
    

> **Готовые конфиги** для Node-RED и примеры кода ESP32 — в соответствующих папках репозитория.