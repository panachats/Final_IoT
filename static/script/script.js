const url = 'http://192.168.0.178:5000/nodemcu/last';
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
    return fetch(url)
        .then(response => response.json())
        .then(data => {
            if (data && data.length > 0) {
                const latestData = data[data.length - 1];
                updateHTML(latestData);
            }
        })
        .catch(error => console.error('Error fetching data:', error));
}

function updateHTML(data) {
    const PM10 = data.PM10
    const PM25 = data.PM25
    const heatIndex = data.HeatIndex;
    const status = data.Status;
    // const heatIndexStatus = heatIndex >= 40 || heatIndex <= 18 || PM25 >= 25 ? 'Uncomfortable' : (heatIndex >= 33 || PM25 >= 12 ? 'Warring' : 'Comfortable');
    cirlePM25.innerHTML = createCircleHTML(PM25, 'cirlePM25');
    cirlePM10.innerHTML = createCircleHTML(PM10, 'cirlePM10');
    switch (status) {
        case 'Uncomfortable':
            displayTempStatus.innerHTML = `<span style="color: #D54949;">${status}</span>`;
            break;
        case 'Warning':
            displayTempStatus.innerHTML = `<span style="color: #D5C749;">${status}</span>`;
            break;
        case 'Comfortable':
            displayTempStatus.innerHTML = `<span style="color: #61D549;">${status}</span>`;
            break;
    }


    displayTimestamp.innerHTML = `Update lastest : ${data.timestamp}`;
    displayTemp.innerHTML = `<b>${data.Temp.toFixed(2)}</b> °C`;
    displayHumid.innerHTML = `<b>${data.Humid.toFixed(2)}</b> %`;
    displayHeatIndex.innerHTML = `<b>${heatIndex.toFixed(2)}</b> °C`;
    displayPM25.innerHTML = `<b>${data.PM25.toFixed(2)}</b> µg/m³`;
    displayPM10.innerHTML = `<b>${data.PM10.toFixed(2)}</b> µg/m³`;
}

function createCircleHTML(value, circleId) {
    let color;
    if (value > 54) {
        color = '#D54949';
    } else if (value >= 35) {
        color = '#D57B49';
    } else if (value >= 12) {
        color = '#D5C749';
    } else {
        color = '#61D549';
    }

    return `<circle cx="30" cy="30" r="20" fill="${color}" id="${circleId}"/>`;
}

fetchData();
setInterval(fetchData, 5000);


// const url = 'http://192.168.0.178:5000/nodemcu/last';
// const displayTemp = document.getElementById('display-Temp');
// const displayHumid = document.getElementById('display-Humid');
// const displayHeatIndex = document.getElementById('display-HeatIndex');
// const displayPM25 = document.getElementById('display-PM25');
// const displayPM10 = document.getElementById('display-PM10');
// const displayTempStatus = document.getElementById('QualityStatus');
// const displayTimestamp = document.getElementById('Displaytime');
// const cirlePM25 = document.getElementById('cirlePM25');
// const cirlePM10 = document.getElementById('cirlePM10');

// function fetchData() {
//     return fetch(url)
//         .then(response => response.json())
//         .then(data => {
//             if (data && data.length > 0) {
//                 const latestData = data[data.length - 1];
//                 updateHTML(latestData);
//             }
//         })
//         .catch(error => console.error('Error fetching data:', error));
// }

// function updateHTML(data) {
//     const PM10 = data.PM10;
//     const PM25 = data.PM25;
//     const heatIndex = data.HeatIndex;
//     const heatIndexStatus = getHeatIndexStatus(heatIndex, PM25);

//     displayTempStatus.innerHTML = `<span style="color: ${getColorByStatus(heatIndexStatus)};">${heatIndexStatus}</span>`;

//     cirlePM25.innerHTML = createCircleHTML(PM25, 'cirlePM25');
//     cirlePM10.innerHTML = createCircleHTML(PM10, 'cirlePM10');

//     displayTimestamp.innerHTML = `Update lastest : ${data.timestamp}`;
//     displayTemp.innerHTML = `<b>${data.Temp.toFixed(2)}</b> °C`;
//     displayHumid.innerHTML = `<b>${data.Humid.toFixed(2)}</b> %`;
//     displayHeatIndex.innerHTML = `<b>${heatIndex.toFixed(2)}</b> °C`;
//     displayPM25.innerHTML = `<b>${PM25.toFixed(2)}</b> µg/m³`;
//     displayPM10.innerHTML = `<b>${PM10.toFixed(2)}</b> µg/m³`;
// }

// function getHeatIndexStatus(heatIndex, PM25) {
//     return heatIndex >= 40 || heatIndex <= 18 || PM25 >= 25 ? 'Uncomfortable' : (heatIndex >= 33 || PM25 >= 12 ? 'Warning' : 'Comfortable');
// }

// function getColorByStatus(status) {
//     const colorMap = {
//         'Uncomfortable': '#D54949',
//         'Warning': '#D5C749',
//         'Comfortable': '#61D549'
//     };
//     return colorMap[status] || '#61D549';
// }

// function createCircleHTML(value, circleId) {
//     let color;
//     if (value > 54) {
//         color = '#D54949';
//     } else if (value >= 35) {
//         color = '#D57B49';
//     } else if (value >= 12) {
//         color = '#D5C749';
//     } else {
//         color = '#61D549';
//     }

//     return `<circle cx="30" cy="30" r="20" fill="${color}" id="${circleId}"/>`;
// }

// fetchData();

// setInterval(fetchData, 5000);
