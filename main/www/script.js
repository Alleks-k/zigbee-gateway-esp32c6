/**
 * Функція оновлення статусу шлюзу та списку пристроїв
 */
function update() {
    fetch('/api/status')
        .then(response => {
            if (!response.ok) {
                throw new Error('Помилка мережі');
            }
            return response.json();
        })
        .then(data => {
            // 1. Оновлюємо блок інформації про шлюз
            const statusDiv = document.getElementById('status');
            if (statusDiv) {
                statusDiv.innerHTML = `
                    <p>PAN ID: <strong>0x${data.pan_id.toString(16).toUpperCase()}</strong></p>
                    <p>Канал: <strong>${data.channel}</strong></p>
                    <p>Адреса: <strong>0x${data.short_addr.toString(16).toUpperCase()}</strong></p>
                `;
            }

            // 2. Оновлюємо список знайдених пристроїв
            const devicesList = document.getElementById('devices');
            if (devicesList) {
                devicesList.innerHTML = ''; // Очищуємо список

                if (!data.devices || data.devices.length === 0) {
                    devicesList.innerHTML = '<li class="empty">Пристроїв не знайдено</li>';
                } else {
                    data.devices.forEach(device => {
                        const li = document.createElement('li');
                        li.className = 'device-item';
                        
                        // Формуємо HEX адресу для відображення
                        const hexAddr = "0x" + device.short_addr.toString(16).toUpperCase();
                        
                        li.innerHTML = `
                            <div class="device-info">
                                <strong>${device.name}</strong>
                                <small>${hexAddr}</small>
                            </div>
                            <div class="device-controls">
                                <button class="btn-on" onclick="control(${device.short_addr}, 1, 1)">Ввімк</button>
                                <button class="btn-off" onclick="control(${device.short_addr}, 1, 0)">Вимк</button>
                            </div>
                        `;
                        devicesList.appendChild(li);
                    });
                }
            }
        })
        .catch(error => {
            console.error('Помилка оновлення:', error);
            const statusDiv = document.getElementById('status');
            if (statusDiv) {
                statusDiv.innerHTML = '<p style="color: red;">Помилка зв\'язку з шлюзом</p>';
            }
        });
}

/**
 * Відкриття мережі для нових пристроїв (Permit Join)
 */
function permitJoin() {
    fetch('/api/permit_join', { 
        method: 'POST' 
    })
    .then(response => {
        if (response.ok) {
            alert('Режим пошуку активовано на 60 секунд. Переведіть ваш Zigbee пристрій у режим сполучення.');
        } else {
            alert('Не вдалося активувати пошук.');
        }
    })
    .catch(error => console.error('Error:', error));
}

/**
 * Відправка команди On/Off на пристрій
 * @param {number} addr - Коротка адреса пристрою
 * @param {number} endpoint - Ендпоінт (зазвичай 1)
 * @param {number} cmd - Команда (1 - On, 0 - Off)
 */
function control(addr, endpoint, cmd) {
    fetch('/api/control', {
        method: 'POST',
        headers: {
            'Content-Type': 'text/plain',
        },
        body: `${addr},${endpoint},${cmd}`
    })
    .then(response => {
        if (!response.ok) {
            alert('Помилка відправки команди');
        }
    })
    .catch(error => console.error('Error:', error));
}

// Запускаємо перше оновлення при завантаженні
update();

// Встановлюємо інтервал оновлення кожні 5 секунд
setInterval(update, 5000);