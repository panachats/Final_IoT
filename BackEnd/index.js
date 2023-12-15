const url = 'http://192.168.0.178:80';
const displayTemp = document.getElementById('display-Temp');
const displayHumid = document.getElementById('display-Humid');
const displayHeatIndex = document.getElementById('display-HeatIndex');
const displayPM25 = document.getElementById('display-PM25');
const displayPM10 = document.getElementById('display-PM10');
const displayTempStatus = document.getElementById('QualityStatus');
const displayTimestamp = document.getElementById('Displaytime');
const cirlePM25 = document.getElementById('cirlePM25');
const cirlePM10 = document.getElementById('cirlePM10');

function fetchData() {
    return fetch(`${url}/ESP8266`)
        .then(response => response.json())
        .then(data => {
            if (data && data.length > 0) {
                const latestData = data[data.length - 1];
                updateHTML(latestData);
            }
        })
        .catch(error => console.error('Error fetching data:', error));
}

function updateCircleElement(element, value, thresholds) {
    let color;
    if (value > thresholds[3]) {
        color = '#D54949';
    } else if (value >= thresholds[2] && value <= thresholds[3]) {
        color = '#D57B49';
    } else if (value >= thresholds[1] && value <= thresholds[2]) {
        color = '#D5C749';
    } else {
        color = '#61D549';
    }

    element.innerHTML = `<circle cx="30" cy="30" r="20" fill="${color}"/>`;
}

function updateHTML(data) {
    const heatIndex = data.HeatIndex;
    const heatIndexStatus = heatIndex > 40 ? 'Bad' : (heatIndex >= 33 && heatIndex <= 39 ? 'Not good' : 'Comfortable');

    switch (heatIndexStatus) {
        case 'Bad':
            displayTempStatus.innerHTML = `<span style="color: #D54949;">${heatIndexStatus}</span>`;
            break;
        case 'Not good':
            displayTempStatus.innerHTML = `<span style="color: #D5C749;">${heatIndexStatus}</span>`;
            break;
        case 'Comfortable':
            displayTempStatus.innerHTML = `<span style="color: #61D549;">${heatIndexStatus}</span>`;
            break;
    }

    updateCircleElement(cirlePM25, data.PM25, [12.1, 35.4, 55.4]);
    updateCircleElement(cirlePM10, data.PM10, [55, 154, 254]);

    displayTimestamp.innerHTML = `Update lastest : ${data.timestamp}`;
    displayTemp.innerHTML = `<b>${data.Temp.toFixed(2)}</b> °C`;
    displayHumid.innerHTML = `<b>${data.Humid.toFixed(2)}</b> %`;
    displayHeatIndex.innerHTML = `<b>${heatIndex.toFixed(2)}</b> °C`;
    displayPM25.innerHTML = `<b>${data.PM25.toFixed(2)}</b> µg/m³`;
    displayPM10.innerHTML = `<b>${data.PM10.toFixed(2)}</b> µg/m³`;
}


// Initial fetch
fetchData();

// Update data every 5000 milliseconds
setInterval(fetchData, 5000);
