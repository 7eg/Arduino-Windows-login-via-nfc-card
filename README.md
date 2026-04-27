# NFC Login Dashboard

> Arduino Leonardo + MFRC522 NFC card reader that logs you into Windows and records every login to a local web dashboard.

---

## Table of Contents / فهرس المحتويات

- [English Documentation](#english-documentation)
- [التوثيق بالعربية](#التوثيق-بالعربية)

---

# English Documentation

## What This Project Does

When you tap your NFC card on the reader:

1. The **Arduino Leonardo** reads the card's unique ID (UID).
2. If the UID matches the authorized card, the Arduino **types your password automatically** using USB keyboard emulation — no manual typing needed.
3. At the same time, it sends a structured event over USB Serial to the **Node.js server** running on your PC.
4. The server saves the login attempt (who, when, granted or denied) to a **SQLite database**.
5. A **live web dashboard** at `http://localhost:3000` displays the full history with stats.

---

## Hardware Required

| Component | Details |
|---|---|
| Arduino Leonardo | Must be Leonardo — it supports USB HID (keyboard emulation). Uno does NOT work. |
| MFRC522 NFC Reader | SPI module, commonly sold with an NFC card and key fob |
| NFC Card / Key Fob | The card you tap to log in |
| Jumper Wires | To connect the reader to the Arduino |

### Wiring (MFRC522 → Arduino Leonardo)

| MFRC522 Pin | Arduino Leonardo Pin |
|---|---|
| SDA (SS) | Pin 10 |
| SCK | Pin 13 (SCK) |
| MOSI | Pin 11 (MOSI) |
| MISO | Pin 12 (MISO) |
| IRQ | Not connected |
| GND | GND |
| RST | Pin 9 |
| 3.3V | 3.3V |

---

## Software Required

- [Arduino IDE](https://www.arduino.cc/en/software) — to upload the sketch
- [Node.js](https://nodejs.org/) (v18 or newer) — to run the server
- Arduino Libraries (install via Library Manager in Arduino IDE):
  - `MFRC522` by GithubCommunity
  - `Keyboard` (built-in)
  - `SPI` (built-in)

---

## Project File Structure

```
New folder (8)/
├── nfc_login.ino     # Arduino sketch — runs on the hardware
├── server.js         # Node.js server — reads serial, saves to DB, serves dashboard
├── index.html        # Web dashboard — live login history
├── package.json      # Node.js dependencies
├── logins.sqlite     # SQLite database (created automatically on first run)
└── README.md         # This file
```

---

## Setup & Installation

### Step 1 — Find Your Card's UID

Before uploading the sketch, you need to know your card's UID.

1. Open `nfc_login.ino` in Arduino IDE.
2. Temporarily comment out the `Keyboard` lines so it only prints to Serial.
3. Upload, open Serial Monitor (9600 baud), and tap your card.
4. The UID will print — copy it (e.g. `B3C95194`).

### Step 2 — Configure the Arduino Sketch

Open `nfc_login.ino` and edit these two lines at the top:

```cpp
const String AUTHORIZED_UID = "B3C95194"; // Your card's UID
const char*  PASSWORD       = "your_password_here"; // Your Windows password
```

### Step 3 — Upload the Sketch

1. Close the Serial Monitor if it is open.
2. Go to **Tools → Board → Arduino Leonardo**.
3. Go to **Tools → Port → COM7** (or whichever COM port your Leonardo appears on in Device Manager).
4. Click **Upload**.

### Step 4 — Install Node.js Dependencies

Open a terminal in the project folder and run:

```bash
npm install
```

### Step 5 — Configure the Server

Open `server.js` and edit the `USERS` map to match your card UID and your name:

```js
const USERS = {
    "B3C95194": "Your Name",
    // Add more cards as needed:
    // "AABBCCDD": "Another Person",
};
```

Also confirm the COM port matches:

```js
const port = new SerialPort({ path: 'COM7', baudRate: 9600 });
```

### Step 6 — Run the Server

```bash
npm start
```

You will see:
```
Serial port open on COM7
Dashboard: http://localhost:3000
```

Open your browser at **http://localhost:3000**.

---

## How to Use

1. Make sure `npm start` is running in the terminal.
2. Lock your Windows screen (Win + L).
3. Tap your NFC card on the reader.
4. The Arduino waits 5 seconds on first power-up (safety delay), then is ready.
5. Windows unlocks automatically, and the dashboard logs the event.

---

## Important Notes

- **Do not have Arduino IDE's Serial Monitor open** while the Node.js server is running — both cannot hold the COM port at the same time. Close Serial Monitor before running `npm start`, and stop the server (`Ctrl+C`) before uploading a new sketch.
- The 5-second startup delay in the Arduino sketch is a safety feature. If the Arduino starts typing unexpectedly, you have 5 seconds to unplug it.
- Passwords are stored in plain text in the Arduino sketch. Do not share the `.ino` file.

---

## Adding More Cards / Users

In `server.js`, add entries to the `USERS` map:

```js
const USERS = {
    "B3C95194": "Rushat",
    "AABBCCDD": "Guest Card",
};
```

In `nfc_login.ino`, you can add multiple authorized UIDs and different passwords per card if needed.

---

## Dashboard Overview

| Column | Description |
|---|---|
| # | Auto-incremented log ID |
| Time | Date and time of the scan (your local timezone) |
| User | Name mapped from the card UID in `server.js` |
| Card UID | The raw hardware ID of the card |
| Status | GRANTED (green) or DENIED (red) |

The stats cards at the top show totals at a glance. The table refreshes every 3 seconds automatically.

---
---

# التوثيق بالعربية

<div dir="rtl">

## ما الذي يفعله هذا المشروع؟

عندما تضع بطاقة NFC على القارئ:

1. يقرأ **Arduino Leonardo** المعرّف الفريد للبطاقة (UID).
2. إذا تطابق المعرّف مع البطاقة المصرّح لها، يقوم الأردوينو **بكتابة كلمة المرور تلقائيًا** عبر محاكاة لوحة المفاتيح USB — دون الحاجة لكتابتها يدويًا.
3. في نفس الوقت، يرسل الأردوينو حدثًا منظّمًا عبر USB Serial إلى **خادم Node.js** الذي يعمل على جهاز الكمبيوتر.
4. يحفظ الخادم محاولة تسجيل الدخول (من هو المستخدم، ووقت المحاولة، وهل تمّ قبولها أم رفضها) في **قاعدة بيانات SQLite**.
5. تعرض **لوحة تحكم ويب مباشرة** على `http://localhost:3000` السجل الكامل مع الإحصائيات.

---

## المكونات المطلوبة

| المكوّن | التفاصيل |
|---|---|
| Arduino Leonardo | يجب أن يكون Leonardo تحديدًا — لأنه يدعم محاكاة لوحة المفاتيح USB HID. لوحة Uno لا تعمل. |
| قارئ NFC MFRC522 | وحدة SPI، تُباع عادةً مع بطاقة NFC ومفتاح صغير |
| بطاقة NFC | البطاقة التي تستخدمها لتسجيل الدخول |
| أسلاك توصيل | لتوصيل القارئ بالأردوينو |

### توصيل الأسلاك (MFRC522 ← Arduino Leonardo)

| طرف MFRC522 | طرف Arduino Leonardo |
|---|---|
| SDA (SS) | المنفذ 10 |
| SCK | المنفذ 13 (SCK) |
| MOSI | المنفذ 11 (MOSI) |
| MISO | المنفذ 12 (MISO) |
| IRQ | غير موصول |
| GND | GND |
| RST | المنفذ 9 |
| 3.3V | 3.3V |

---

## البرامج المطلوبة

- [Arduino IDE](https://www.arduino.cc/en/software) — لرفع الكود على الأردوينو
- [Node.js](https://nodejs.org/) (الإصدار 18 أو أحدث) — لتشغيل الخادم
- مكتبات الأردوينو (تُثبَّت من Library Manager داخل Arduino IDE):
  - `MFRC522` من مجتمع GitHub
  - `Keyboard` (مدمجة)
  - `SPI` (مدمجة)

---

## هيكل ملفات المشروع

```
المجلد/
├── nfc_login.ino     # كود الأردوينو — يعمل على الجهاز مباشرة
├── server.js         # خادم Node.js — يقرأ المنفذ التسلسلي ويحفظ البيانات ويعرض الصفحة
├── index.html        # لوحة التحكم — سجل تسجيل الدخول المباشر
├── package.json      # اعتماديات Node.js
├── logins.sqlite     # قاعدة البيانات SQLite (تُنشأ تلقائيًا عند أول تشغيل)
└── README.md         # هذا الملف
```

---

## الإعداد والتثبيت

### الخطوة الأولى — معرفة معرّف بطاقتك (UID)

قبل رفع الكود، تحتاج إلى معرفة المعرّف الفريد لبطاقتك.

1. افتح `nfc_login.ino` في Arduino IDE.
2. ارفع الكود، افتح Serial Monitor (9600 baud)، ثم ضع البطاقة على القارئ.
3. سيُطبع المعرّف — انسخه (مثال: `B3C95194`).

### الخطوة الثانية — ضبط إعدادات الأردوينو

افتح `nfc_login.ino` وعدّل هذين السطرين في الأعلى:

```cpp
const String AUTHORIZED_UID = "B3C95194"; // معرّف بطاقتك
const char*  PASSWORD       = "كلمة_المرور_هنا"; // كلمة مرور Windows
```

### الخطوة الثالثة — رفع الكود على الأردوينو

1. أغلق Serial Monitor إذا كان مفتوحًا.
2. اختر **Tools → Board → Arduino Leonardo**.
3. اختر **Tools → Port → COM7** (أو المنفذ الذي يظهر عليه الأردوينو في Device Manager).
4. اضغط على زر **Upload**.

### الخطوة الرابعة — تثبيت اعتماديات Node.js

افتح Terminal في مجلد المشروع وشغّل:

```bash
npm install
```

### الخطوة الخامسة — ضبط إعدادات الخادم

افتح `server.js` وعدّل خريطة `USERS` لتتناسب مع معرّف بطاقتك واسمك:

```js
const USERS = {
    "B3C95194": "اسمك",
    // يمكنك إضافة بطاقات أخرى:
    // "AABBCCDD": "شخص آخر",
};
```

### الخطوة السادسة — تشغيل الخادم

```bash
npm start
```

ستظهر الرسالة:
```
Serial port open on COM7
Dashboard: http://localhost:3000
```

افتح المتصفح على **http://localhost:3000**.

---

## كيفية الاستخدام

1. تأكد من أن `npm start` يعمل في Terminal.
2. اقفل شاشة Windows (Win + L).
3. ضع بطاقة NFC على القارئ.
4. عند أول تشغيل للأردوينو، ينتظر 5 ثوانٍ (وقت الأمان)، ثم يصبح جاهزًا.
5. يُفتح Windows تلقائيًا، وتُسجَّل العملية في لوحة التحكم.

---

## ملاحظات مهمة

- **لا تفتح Serial Monitor في Arduino IDE** أثناء تشغيل خادم Node.js — لا يمكن لكليهما الاحتفاظ بالمنفذ التسلسلي في نفس الوقت. أغلق Serial Monitor قبل تشغيل `npm start`، وأوقف الخادم بـ `Ctrl+C` قبل رفع كود جديد.
- تأخير 5 ثوانٍ عند بدء التشغيل هو ميزة أمان. إذا بدأ الأردوينو بالكتابة بشكل غير متوقع، لديك 5 ثوانٍ لفصله.
- كلمة المرور محفوظة كنص عادي في ملف الأردوينو. لا تشارك ملف `.ino` مع أي شخص.

---

## إضافة بطاقات ومستخدمين جدد

في `server.js`، أضف مدخلات إلى خريطة `USERS`:

```js
const USERS = {
    "B3C95194": "رشات",
    "AABBCCDD": "بطاقة ضيف",
};
```

---

## شرح لوحة التحكم

| العمود | الوصف |
|---|---|
| # | رقم السجل التلقائي |
| Time | تاريخ ووقت المسح (بالتوقيت المحلي) |
| User | الاسم المرتبط بمعرّف البطاقة في `server.js` |
| Card UID | المعرّف الفريد للبطاقة من الجهاز |
| Status | GRANTED باللون الأخضر أو DENIED باللون الأحمر |

تُعرض الإحصائيات الإجمالية في الأعلى، وتتحدث البيانات تلقائيًا كل 3 ثوانٍ.

</div>
