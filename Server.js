const http = require('http');

// Global state to store the latest sensor reading for the web dashboard
let latestDistance = 300;

const server = http.createServer((req, res) => {
    // Enable CORS so your frontend dashboard can fetch data without browser blocks
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

    // Handle browser preflight CORS check
    if (req.method === 'OPTIONS') {
        res.statusCode = 204;
        return res.end();
    }

    // Route 1: Receive data from ESP32 sensor
    if (req.method === 'POST' && req.url === '/api/sensor') {
        let body = ''; // Local stream buffer per request

        req.on('data', (chunk) => {
            body += chunk;
        });

        req.on('end', () => {
            try {
                const dataRecues = body ? JSON.parse(body) : null;

                // Extract distance from JSON payload (supports both 'Distance' and 'distance')
                if (dataRecues) {
                    latestDistance = dataRecues.Distance ?? dataRecues.distance ?? latestDistance;
                }

                console.log(`[ESP32] Distance received: ${latestDistance} cm`);

                res.statusCode = 200;
                res.setHeader('Content-Type', 'application/json');
                res.end(JSON.stringify({
                    message: 'Data Received',
                    data: dataRecues,
                }));
            } catch (err) {
                res.statusCode = 400;
                res.setHeader('Content-Type', 'application/json');
                res.end(JSON.stringify({ error: 'Invalid JSON payload' }));
            }
        });

        // Route 2: Send latest distance to website frontend
    } else if (req.method === 'GET' && req.url === '/api/distance') {
        res.statusCode = 200;
        res.setHeader('Content-Type', 'application/json');
        res.end(JSON.stringify({
            distance: latestDistance
        }));

        // Route 3: Unknown routes fallback
    } else {
        res.statusCode = 404;
        res.setHeader('Content-Type', 'application/json');
        res.end(JSON.stringify({ erreur: 'Route non trouvée' }));
    }
});

const PORT = 5000;
server.listen(PORT, () => {
    console.log(`Server running on http://localhost:${PORT}`);
});