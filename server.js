import { SerialPort } from 'serialport';
import { ReadlineParser } from '@serialport/parser-readline';
import express from 'express';
import sqlite3 from 'sqlite3';
import { fileURLToPath } from 'url';
import { dirname, join } from 'path';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

// --- MAP YOUR CARD UIDs TO NAMES HERE ---
const USERS = {
    "B3C95194": "Rushat",
    // Add more cards: "AABBCCDD": "Another Person",
};
// -----------------------------------------

const app = express();
const db = new sqlite3.Database('./logins.sqlite');

db.serialize(() => {
    // Create table with base schema (compatible with old databases)
    db.run(`CREATE TABLE IF NOT EXISTS logs (
        id     INTEGER PRIMARY KEY AUTOINCREMENT,
        uid    TEXT    NOT NULL,
        status TEXT    NOT NULL,
        time   DATETIME DEFAULT CURRENT_TIMESTAMP
    )`);
    // Add username column if this is an older database — error is expected and ignored when column already exists
    db.run(`ALTER TABLE logs ADD COLUMN username TEXT DEFAULT 'Unknown'`, (err) => {
        if (err && !err.message.includes('duplicate column')) {
            console.error('DB migration error:', err.message);
        }
    });
});

// Setup Serial Port — change COM7 to match your Device Manager
const port = new SerialPort({ path: 'COM7', baudRate: 9600 });
const parser = port.pipe(new ReadlineParser({ delimiter: '\r\n' }));

port.on('open', () => console.log('Serial port open on COM7'));
port.on('error', (err) => console.error('Serial port error:', err.message));

parser.on('data', (line) => {
    console.log('[Arduino]', line);

    if (!line.startsWith('AUTH_EVENT:')) return;

    // Format sent by Arduino:  AUTH_EVENT:<UID>:<GRANTED|DENIED>
    const parts = line.split(':');
    if (parts.length < 3) return;

    const uid      = parts[1].trim().toUpperCase();
    const status   = parts[2].trim().toUpperCase(); // "GRANTED" or "DENIED"
    const username = USERS[uid] ?? 'Unknown';

    console.log(`[Card] UID: ${uid} | User: ${username} | Status: ${status}`);

    db.run(
        'INSERT INTO logs (uid, username, status) VALUES (?, ?, ?)',
        [uid, username, status],
        (err) => { if (err) console.error('DB Error:', err.message); }
    );
});

// Web routes
app.use(express.static(__dirname));
app.get('/', (req, res) => res.sendFile(join(__dirname, 'index.html')));

app.get('/api/logs', (req, res) => {
    db.all('SELECT * FROM logs ORDER BY time DESC LIMIT 100', [], (err, rows) => {
        if (err) return res.status(500).json({ error: err.message });
        res.json(rows);
    });
});

app.get('/api/stats', (req, res) => {
    db.get(
        `SELECT
            COUNT(*) AS total,
            SUM(CASE WHEN status = 'GRANTED' THEN 1 ELSE 0 END) AS granted,
            SUM(CASE WHEN status = 'DENIED'  THEN 1 ELSE 0 END) AS denied
         FROM logs`,
        [],
        (err, row) => {
            if (err) return res.status(500).json({ error: err.message });
            res.json(row);
        }
    );
});

const PORT = 3000;
app.listen(PORT, () => {
    console.log(`Dashboard: http://localhost:${PORT}`);
    console.log('Listening for Arduino on COM7...');
});
