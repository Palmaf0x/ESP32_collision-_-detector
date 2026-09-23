const distDisplay = document.getElementById('dist-display');
const statusText = document.getElementById('status-text');
const statusDot = document.getElementById('status-dot');
const alertIcon = document.getElementById('alert-icon');
const radarContainer = document.getElementById('radar-bars');
const systemStatus = document.getElementById('system-status');

// Initialize radar bars
const numBars = 12;
for(let i = 0; i < numBars; i++) {
    const bar = document.createElement('div');
    bar.className = 'radar-bar';
    bar.style.height = `${(i + 1) * 5}px`;
    radarContainer.appendChild(bar);
}
const bars = document.querySelectorAll('.radar-bar');

// State Management for Interpolation
let currentDistance = 150; // Visual starting point
let targetDistance = 150;  // Value updated by server
const SMOOTHING_FACTOR = 0.1; // Adjust between 0.01 (slow) and 0.5 (fast)

// Core UI Update Logic
function updateDashboard(distance) {
    distDisplay.innerText = distance.toString().padStart(3, '0');

    const CRITICAL_DIST = 30;
    const WARNING_DIST = 100;
    let activeColor = '';

    if (distance <= CRITICAL_DIST) {
        activeColor = 'var(--accent-red)';
        statusText.innerText = 'CRITICAL';
        statusText.style.color = activeColor;
        statusDot.style.backgroundColor = activeColor;
        statusDot.style.boxShadow = `0 0 10px ${activeColor}`;
        alertIcon.style.display = 'block';
        systemStatus.innerText = 'Collision Imminent';
        systemStatus.style.color = activeColor;
    } else if (distance <= WARNING_DIST) {
        activeColor = 'var(--accent-orange)';
        statusText.innerText = 'WARNING';
        statusText.style.color = activeColor;
        statusDot.style.backgroundColor = activeColor;
        statusDot.style.boxShadow = `0 0 10px ${activeColor}`;
        alertIcon.style.display = 'none';
        systemStatus.innerText = 'Object Detected';
        systemStatus.style.color = activeColor;
    } else {
        activeColor = 'var(--accent-green)';
        statusText.innerText = 'SAFE';
        statusText.style.color = activeColor;
        statusDot.style.backgroundColor = activeColor;
        statusDot.style.boxShadow = `0 0 10px ${activeColor}`;
        alertIcon.style.display = 'none';
        systemStatus.innerText = 'System Operational';
        systemStatus.style.color = 'var(--accent-orange)';
    }

    const activeBars = Math.ceil((distance / 300) * numBars);
    bars.forEach((bar, index) => {
        if (index < activeBars) {
            bar.style.backgroundColor = '#333';
        } else {
            bar.style.backgroundColor = activeColor;
        }
    });
}

// Render Loop for Smooth Transitions
function animateUI() {
    // Check if interpolation is needed
    if (Math.abs(targetDistance - currentDistance) > 0.5) {
        // Move current value closer to target value
        currentDistance += (targetDistance - currentDistance) * SMOOTHING_FACTOR;
    } else {
        // Snap to target to avoid micro-calculations
        currentDistance = targetDistance;
    }

    // Update the DOM with the rounded interpolated value
    updateDashboard(Math.round(currentDistance));

    // Request the next frame
    requestAnimationFrame(animateUI);
}

// HTTP Polling Logic
async function pollServer() {
    try {
        // Replace with your local Node.js / Express server endpoint
        const response = await fetch('http://localhost:3000/api/distance');

        if (!response.ok) {
            throw new Error(`HTTP Error: ${response.status}`);
        }

        const data = await response.json();

        // Update the target, allowing animateUI() to handle the smooth visual transition
        if(typeof data.distance === 'number') {
            targetDistance = data.distance;
        }
    } catch (error) {
        console.error("Data fetch failed:", error);
    }
}

// Initialization
requestAnimationFrame(animateUI); // Start visual rendering loop
setInterval(pollServer, 5000);     // Poll the server every 5000ms