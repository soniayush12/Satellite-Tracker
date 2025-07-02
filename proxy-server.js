const express = require('express');
const path = require('path');
const fetch = (...args) => import('node-fetch').then(({default: fetch}) => fetch(...args));

const app = express();
const PORT = 3000;
const API_KEY = 'F4WFLX-KCQFVS-TTXZ7X-5IQQ';

// Allow CORS
app.use((req, res, next) => {
  res.header('Access-Control-Allow-Origin', '*');
  next();
});

// ✅ Serve static files from current directory
app.use(express.static(path.join(__dirname)));

// Proxy route
app.get('/satellite/:id', async (req, res) => {
  const { id } = req.params;
  const url = `https://api.n2yo.com/rest/v1/satellite/positions/${id}/28.6139/77.2090/0/1/?apiKey=${API_KEY}`;
  try {
    const response = await fetch(url);
    if (!response.ok) {
      const text = await response.text();
      console.error(`[${id}] N2YO Error ${response.status}:`, text);
      return res.status(response.status).send({ error: text });
    }
    const data = await response.json();
    res.json(data);
  } catch (err) {
    console.error(`[${id}] Fetch failed:`, err.message);
    res.status(500).send({ error: err.message });
  }
});

app.listen(PORT, () => console.log(`Proxy running at http://localhost:${PORT}`));
