/**
 * Wall-E OS NFC Handler, Web Audio Controller & ESP32 Hardware Integrator
 * Compatible con: ESP32 Hardware (http://192.168.1.72), Web NFC Android y Teclado/USB
 */

class WalleNFCController {
  constructor() {
    this.modoActual = 'prestamo'; // 'prestamo' or 'devolucion'
    this.alumnoUidMemoria = null;
    this.ultimoUIDLeidoESP32 = "";

    this.lcdScreen = document.getElementById('lcd-screen');
    this.lcdText = document.getElementById('lcd-text');
    this.nfcInput = document.getElementById('nfc-input');
    this.nfcForm = document.getElementById('nfc-form');
    this.logConsole = document.getElementById('terminal-logs');
    this.nfcStatusBadge = document.getElementById('nfcStatusBadge');
    this.nfcStatusText = document.getElementById('nfcStatusText');

    this.btnPrestamo = document.getElementById('btn-prestamo');
    this.btnDevolucion = document.getElementById('btn-devolucion');

    this.initAudioContext();
    this.initEvents();
    this.initAndroidNFC();
    this.initESP32HardwareListener(); // 🚀 Iniciar conexión en segundo plano con ESP32 (http://192.168.1.72)
    this.setModo('prestamo');
  }

  initAudioContext() {
    this.audioCtx = null;
    // Audio pre-cargado de WALL-E (voz y efectos)
    this.audioWallE = new Audio('https://ia800204.us.archive.org/24/items/wall-e-sound-effects/WALL-E%20Wall-E.mp3');
    this.audioExito = new Audio('https://ia600204.us.archive.org/24/items/wall-e-sound-effects/WALL-E%20Ta-Da.mp3');

    const initAudio = () => {
      if (!this.audioCtx) {
        this.audioCtx = new (window.AudioContext || window.webkitAudioContext)();
      }
    };
    window.addEventListener('click', initAudio, { once: true });
    window.addEventListener('touchstart', initAudio, { once: true });
    window.addEventListener('keydown', initAudio, { once: true });
  }

  playWallESound() {
    if (this.audioWallE) {
      this.audioWallE.currentTime = 0;
      this.audioWallE.play().catch(err => {
        console.log("Audio de WALL-E bloqueado por el navegador. Haga clic en la página.");
      });
    }
  }

  playSound(tipo) {
    if (tipo === 'walle' || tipo === 'exito') {
      this.playWallESound();
    }

    const audioPath = tipo === 'exito' ? '/static/sonidos/exito.mp3' : '/static/sonidos/error.mp3';
    const audio = new Audio(audioPath);
    audio.play().catch(() => {
      if (!this.audioCtx) return;
      const osc = this.audioCtx.createOscillator();
      const gain = this.audioCtx.createGain();
      osc.connect(gain);
      gain.connect(this.audioCtx.destination);

      if (tipo === 'exito' || tipo === 'walle') {
        osc.type = 'sine';
        osc.frequency.setValueAtTime(587.33, this.audioCtx.currentTime);
        osc.frequency.setValueAtTime(880, this.audioCtx.currentTime + 0.1);
        gain.gain.setValueAtTime(0.3, this.audioCtx.currentTime);
        gain.gain.exponentialRampToValueAtTime(0.01, this.audioCtx.currentTime + 0.35);
        osc.start();
        osc.stop(this.audioCtx.currentTime + 0.35);
      } else {
        osc.type = 'sawtooth';
        osc.frequency.setValueAtTime(160, this.audioCtx.currentTime);
        osc.frequency.setValueAtTime(110, this.audioCtx.currentTime + 0.15);
        gain.gain.setValueAtTime(0.3, this.audioCtx.currentTime);
        gain.gain.exponentialRampToValueAtTime(0.01, this.audioCtx.currentTime + 0.4);
        osc.start();
        osc.stop(this.audioCtx.currentTime + 0.4);
      }
    });
  }

  log(mensaje) {
    if (!this.logConsole) return;
    const now = new Date();
    const timestamp = now.toTimeString().split(' ')[0];
    const logLine = document.createElement('div');
    logLine.className = 'log-entry';
    logLine.innerHTML = `<span class="log-timestamp">[${timestamp}]</span> ${mensaje}`;
    this.logConsole.appendChild(logLine);
    this.logConsole.scrollTop = this.logConsole.scrollHeight;
  }

  setModo(modo) {
    this.modoActual = modo;
    this.alumnoUidMemoria = null;

    if (modo === 'prestamo') {
      if (this.btnPrestamo) this.btnPrestamo.classList.add('active');
      if (this.btnDevolucion) this.btnDevolucion.classList.remove('active');
      this.updateLCD("MODO PRÉSTAMO:\nESPERANDO CREDENCIAL (ADDR04)...", 'normal');
      this.log("=== MODO CAMBIADO A: PRÉSTAMO ===");
    } else {
      if (this.btnDevolucion) this.btnDevolucion.classList.add('active');
      if (this.btnPrestamo) this.btnPrestamo.classList.remove('active');
      this.updateLCD("MODO DEVOLUCIÓN:\nESCANEE ETIQUETA DE LIBRO (ADDR04)...", 'normal');
      this.log("=== MODO CAMBIADO A: DEVOLUCIÓN ===");
    }

    if (this.nfcInput) {
      this.nfcInput.focus();
    }
  }

  updateLCD(texto, estado = 'normal') {
    if (!this.lcdScreen || !this.lcdText) return;
    this.lcdText.textContent = texto;
    this.lcdScreen.className = 'lcd-screen-container';
    
    if (estado === 'exito') {
      this.lcdScreen.classList.add('lcd-state-success');
    } else if (estado === 'error') {
      this.lcdScreen.classList.add('lcd-state-error');
    }
  }

  initEvents() {
    if (this.btnPrestamo) {
      this.btnPrestamo.addEventListener('click', () => this.setModo('prestamo'));
    }
    if (this.btnDevolucion) {
      this.btnDevolucion.addEventListener('click', () => this.setModo('devolucion'));
    }

    if (this.nfcForm) {
      this.nfcForm.addEventListener('submit', (e) => {
        e.preventDefault();
        const uid = this.nfcInput.value.trim();
        if (uid) {
          this.procesarNFC(uid);
          this.nfcInput.value = '';
        }
      });
    }

    document.addEventListener('click', (e) => {
      if (this.nfcInput && !['INPUT', 'TEXTAREA', 'BUTTON', 'A'].includes(e.target.tagName)) {
        this.nfcInput.focus();
      }
    });
  }

  // 📡 CONEXIÓN CON ESP32 HARDWARE EN RED LOCAL (mDNS http://Wall-e.local o IP Configurable)
  async initESP32HardwareListener() {
    this.reconstruirHostsESP32();

    const consultarESP32 = async () => {
      if (!this.hostsParaProbar || this.hostsParaProbar.length === 0) {
        this.reconstruirHostsESP32();
      }

      const hostActual = this.hostsParaProbar[this.hostActivoIndex];
      if (!hostActual) return;

      const urlHost = hostActual.startsWith('http') ? hostActual : `http://${hostActual}`;

      try {
        const res = await fetch(`${urlHost}/api/status?t=` + new Date().getTime(), { signal: AbortSignal.timeout(1500) });
        const data = await res.json();

        // Si el ESP32 respondió correctamente, actualizar el indicador de estado
        if (this.nfcStatusBadge && this.nfcStatusText) {
          this.nfcStatusBadge.querySelector('.status-dot').className = "status-dot green";
          const hostLimpio = urlHost.replace('http://', '').replace('/', '');
          this.nfcStatusText.innerText = `📡 ESP32 Conectado (${hostLimpio})`;
        }

        // Si hay un nuevo UID escaneado por el PN532
        if (data.uid && data.uid !== "Ninguna tarjeta escaneada" && data.uid !== this.ultimoUIDLeidoESP32) {
          this.ultimoUIDLeidoESP32 = data.uid;
          
          this.log(`🤖 [ESP32 PN532 HARDWARE] Tarjeta detectada! HEX: ${data.uid} | DEC: ${data.decimal}`);
          
          // 🔊 Reproducir Sonido WALL-E
          this.playWallESound();

          // Usar el valor decimal o hexadecimal para procesar en Django
          const uidParaProcesar = (data.decimal && data.decimal !== "---") ? data.decimal : data.uid;
          
          if (this.nfcInput) {
            this.nfcInput.value = uidParaProcesar;
          }

          this.procesarNFC(uidParaProcesar);
        }
      } catch (err) {
        // Probar el siguiente host en la lista si falló la conexión
        this.hostActivoIndex = (this.hostActivoIndex + 1) % this.hostsParaProbar.length;
        if (this.nfcStatusBadge && this.nfcStatusText && this.hostActivoIndex === 0) {
          this.nfcStatusBadge.querySelector('.status-dot').className = "status-dot yellow";
          this.nfcStatusText.innerText = "📡 Buscando ESP32 en la red local (192.168.4.1 / Wall-e.local)...";
        }
      }
    };

    // Consultar el estado del ESP32 cada 350 ms
    setInterval(consultarESP32, 350);
  }

  reconstruirHostsESP32() {
    const customIp = (localStorage.getItem('esp32_ip') || '').trim();
    const listaBase = [
      customIp,
      '192.168.4.1',
      'Wall-e.local',
      'guali.local',
      '192.168.1.72',
      '192.168.0.72',
      '192.168.1.100'
    ];

    // Filtrar elementos vacíos y eliminar duplicados manteniendo el orden
    this.hostsParaProbar = [...new Set(listaBase.filter(h => h && h.length > 0))];
    this.hostActivoIndex = 0;
  }

  configurarIPESP32(nuevaIP) {
    if (!nuevaIP) return;
    const ipLimpia = nuevaIP.trim().replace(/^https?:\/\//, '').replace(/\/.*$/, '');
    if (ipLimpia) {
      localStorage.setItem('esp32_ip', ipLimpia);
      this.reconstruirHostsESP32();
      this.log(`⚙️ IP del ESP32 configurada manualmente a: http://${ipLimpia}`);
      this.updateLCD(`CONFIGURANDO ESP32...\nIP: ${ipLimpia}`, 'normal');
    }
  }


  async initAndroidNFC() {
    if ('NDEFReader' in window) {
      if (this.nfcStatusBadge && this.nfcStatusText) {
        this.nfcStatusBadge.querySelector('.status-dot').className = "status-dot green";
        this.nfcStatusText.innerText = "Lector NFC Android y ESP32 Activos";
      }
      try {
        const ndef = new NDEFReader();
        await ndef.scan();
        this.log("[NFC ANDROID] Lector NFC iniciado correctamente en Chrome.");
        
        ndef.onreading = ({ message, serialNumber }) => {
          let memoryId = null;
          if (message && message.records) {
            for (const record of message.records) {
              if (record.data) {
                const decoder = new TextDecoder(record.encoding || 'utf-8');
                const content = decoder.decode(record.data).trim().toUpperCase();
                if (content.match(/^[0-9A-F:]+$/)) {
                  memoryId = content.replace(/:/g, '');
                  break;
                }
              }
            }
          }
          const finalId = memoryId || (serialNumber ? serialNumber.replace(/:/g, '').toUpperCase() : '');
          if (finalId) {
            this.playWallESound();
            this.procesarNFC(finalId);
          }
        };

        ndef.onreadingerror = () => {
          this.log("[ERR NFC] Error de lectura en el chip NFC de Android.");
          this.updateLCD("ERR: LECTURA NFC FALLIDA", 'error');
          this.playSound('error');
        };
      } catch (err) {
        this.log(`[WARN NFC] Toque la pantalla para activar el lector NFC Android: ${err.message}`);
      }
    }
  }

  async procesarNFC(nfcUid) {
    this.log(`--> [NFC LEÍDO]: '${nfcUid.toUpperCase()}'`);
    this.updateLCD(`PROCESANDO CÓDIGO NFC...\n[${nfcUid.toUpperCase()}]`, 'normal');

    try {
      const response = await fetch('/api/nfc/scan/', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          nfc_uid: nfcUid,
          modo: this.modoActual,
          alumno_uid_memoria: this.alumnoUidMemoria
        })
      });

      const res = await response.json();

      if (res.status === 'success') {
        this.playWallESound();
        this.updateLCD(res.message, 'exito');
        this.log(`   ↳ [OK] ${res.message.replace(/\n/g, ' | ')}`);

        if (res.type === 'alumno') {
          this.alumnoUidMemoria = res.alumno_uid;
          this.log(`   ↳ [MEMORIA] Alumno activo: ${res.nombre}`);
        } else if (res.type === 'prestamo_completado' || res.type === 'devolucion_completada') {
          this.alumnoUidMemoria = null;
          setTimeout(() => location.reload(), 1500);
        }
      } else {
        this.playSound('error');
        this.updateLCD(res.message, 'error');
        this.log(`   ↳ [ERR] ${res.message.replace(/\n/g, ' | ')}`);
        
        if (res.type === 'libro' && res.message.includes('ESCANEE CREDENCIAL')) {
          this.alumnoUidMemoria = null;
        }
      }

    } catch (err) {
      this.playSound('error');
      this.updateLCD("ERR: ERROR DE CONEXIÓN CON SERVIDOR", 'error');
      this.log(`   ↳ [ERR SERVIDOR]: ${err.message}`);
    }
  }
}

document.addEventListener('DOMContentLoaded', () => {
  window.walleNfcApp = new WalleNFCController();
});
