#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

#include <Arduino.h>

const char web_U_boat_html[] PROGMEM = R"rawliteral(
   <!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>潜水艇控制面板</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet">
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.3/font/bootstrap-icons.min.css">
    <style>
        :root {
            --primary-color: #4CAF50;
            --secondary-color: #6c757d;
            --success-color: #28a745;
            --danger-color: #dc3545;
            --warning-color: #ffc107;
            --info-color: #17a2b8;
            --dark-color: #343a40;
        }
        body {
            background-color: #f8f9fa;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        }
        .connection-status {
            position: absolute;
            top: 10px;
            left: 10px;
            font-size: 14px;
        }
        .control-panel {
            background: white;
            border-radius: 10px;
            box-shadow: 0 0 20px rgba(0,0,0,0.1);
            padding: 25px;
            margin-top: 30px;
        }
        .status-card {
            border-left: 4px solid var(--primary-color);
            transition: all 0.3s;
        }
        .status-card:hover {
            transform: translateY(-5px);
            box-shadow: 0 10px 20px rgba(0,0,0,0.1);
        }
        .btn-command {
            min-width: 100px;
            margin: 5px;
        }
        .log-container {
            height: 300px;
            overflow-y: auto;
            background-color: #f1f1f1;
            border-radius: 5px;
            padding: 15px;
        }
        .log-entry {
            border-bottom: 1px solid #ddd;
            padding: 5px 0;
            font-family: monospace;
        }
        .mode-indicator {
            width: 15px;
            height: 15px;
            border-radius: 50%;
            display: inline-block;
            margin-right: 5px;
        }
        .mode-real {
            background-color: var(--danger-color);
        }
        .mode-simulated {
            background-color: var(--success-color);
        }
        .mode-manual {
            background-color: var(--info-color);
        }
        .mode-auto {
            background-color: var(--warning-color);
        }
        .video-container {
            height: 240px;
            background-color: #000;
            border-radius: 5px;
            overflow: hidden;
            position: relative;
        }
        #videoFeed {
            width: 100%;
            height: 100%;
            object-fit: cover;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="row">
            <div class="col-md-12 text-center my-4">
                <h1><i class="bi bi-water"></i> 潜水艇控制面板</h1>
                <p class="text-muted">ESP硬件控制界面</p>
            </div>
        </div>

        <div class="row">
            <div class="col-md-4">
                <div class="control-panel">
                    <h4><i class="bi bi-gear"></i> 控制模式</h4>
                    <div class="form-check form-switch mb-3">
                        <input class="form-check-input" type="checkbox" id="modeSwitch" onchange="toggleMode()">
                        <label class="form-check-label" for="modeSwitch">
                            <span id="modeText">手动模式</span>
                            <span id="modeIndicator" class="mode-indicator mode-manual"></span>
                        </label>
                    </div>
                    
                    <div id="manualControl">
                        <h4><i class="bi bi-controller"></i> 手动控制</h4>
                        <div class="d-flex flex-wrap">
                            <button class="btn btn-primary btn-command" onclick="sendCommand('forward')">
                                <i class="bi bi-arrow-up"></i> 前进
                            </button>
                            <button class="btn btn-primary btn-command" onclick="sendCommand('backward')">
                                <i class="bi bi-arrow-down"></i> 后退
                            </button>
                            <button class="btn btn-primary btn-command" onclick="sendCommand('left')">
                                <i class="bi bi-arrow-left"></i> 左转
                            </button>
                            <button class="btn btn-primary btn-command" onclick="sendCommand('right')">
                                <i class="bi bi-arrow-right"></i> 右转
                            </button>
                            <button class="btn btn-primary btn-command" onclick="sendCommand('dive')">
                                <i class="bi bi-arrow-down-circle"></i> 下潜
                            </button>
                            <button class="btn btn-primary btn-command" onclick="sendCommand('rise')">
                                <i class="bi bi-arrow-up-circle"></i> 上浮
                            </button>
                            <button class="btn btn-warning btn-command" onclick="sendCommand('stop')">
                                <i class="bi bi-stop-circle"></i> 停止
                            </button>
                            <button class="btn btn-success btn-command" onclick="sendCommand('grab')">
                                <i class="bi bi-hand-index"></i> 抓取
                            </button>
                            <button class="btn btn-danger btn-command" onclick="sendCommand('release')">
                                <i class="bi bi-hand-thumbs-up"></i> 释放
                            </button>
                        </div>
                    </div>
                    
                    <div id="autoControl" style="display:none;">
                        <h4><i class="bi bi-robot"></i> 自动控制</h4>
                        <div class="d-flex flex-wrap">
                            <button class="btn btn-primary btn-command" onclick="sendCommand('hover')">
                                <i class="bi bi-pause-circle"></i> 悬停
                            </button>
                            <button class="btn btn-primary btn-command" onclick="sendCommand('lock')">
                                <i class="bi bi-lock"></i> 锁定
                            </button>
                            <button class="btn btn-danger btn-command" onclick="sendCommand('fire')">
                                <i class="bi bi-rocket"></i> 发射
                            </button>
                            <button class="btn btn-info btn-command" onclick="sendCommand('self_check')">
                                <i class="bi bi-clipboard-check"></i> 自检状态
                            </button>
                        </div>
                    </div>
                </div>

                <!-- 新增视频模块 -->
                <div class="control-panel mt-3">
                    <h4><i class="bi bi-camera-video"></i> 视频监控</h4>
                    <div class="mb-3">
                        <label for="videoIP" class="form-label">视频流地址</label>
                        <div class="input-group">
                            <input type="text" class="form-control" id="videoIP" placeholder="例如: rtsp://192.168.1.100:554/stream">
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-md-6">
                            <label for="videoUsername" class="form-label">用户名</label>
                            <input type="text" class="form-control" id="videoUsername" placeholder="视频流用户名">
                        </div>
                        <div class="col-md-6">
                            <label for="videoPassword" class="form-label">密码</label>
                            <input type="password" class="form-control" id="videoPassword" placeholder="视频流密码">
                        </div>
                    </div>
                    <button class="btn btn-primary w-100 mt-2" onclick="connectVideo()">连接</button>
                    <div class="video-container">
                        <video id="videoFeed" width="100%" height="240" controls style="display:none;"></video>
                        <div id="videoPlaceholder" class="text-center py-4 bg-light rounded">
                            <i class="bi bi-camera-off fs-1"></i>
                            <p class="mt-2">未连接视频流</p>
                        </div>
                    </div>
                </div>
            </div>

            <div class="col-md-4">
                <div class="control-panel">
                    <h4><i class="bi bi-speedometer2"></i> 状态监控</h4>
                    <div id="statusInfo">
                        <div class="card status-card mb-3">
                            <div class="card-body">
                                <h5 class="card-title"><i class="bi bi-cpu"></i> 硬件状态</h5>
                                <p class="card-text" id="hardwareStatus">加载中...</p>
                                <div class="progress mt-2">
                                    <div id="cpuUsage" class="progress-bar bg-info" role="progressbar"></div>
                                </div>
                            </div>
                        </div>
                        <div class="card status-card mb-3">
                            <div class="card-body">
                                <h5 class="card-title"><i class="bi bi-lightning-charge"></i> 动力系统</h5>
                                <p class="card-text" id="motorStatus">电压: <span id="voltage">--</span>V</p>
                                <p class="card-text" id="currentStatus">电流: <span id="current">--</span>A</p>
                            </div>
                        </div>
                        <div class="card status-card mb-3">
                            <div class="card-body">
                                <h5 class="card-title"><i class="bi bi-water"></i> 环境数据</h5>
                                <p class="card-text">深度: <span id="depth">--</span>米</p>
                                <p class="card-text">温度: <span id="temperature">--</span>°C</p>
                            </div>
                        </div>
                        <button class="btn btn-danger w-100 mb-2" onclick="emergencyStop()">
                            <i class="bi bi-exclamation-triangle"></i> 紧急停止
                        </button>
                        <button class="btn btn-info w-100" onclick="refreshStatus()">
                            <i class="bi bi-arrow-clockwise"></i> 刷新状态
                        </button>
                    </div>
                </div>
            </div>

            <div class="col-md-4">
                <div class="control-panel">
                    <h4><i class="bi bi-terminal"></i> 自定义命令</h4>
                    <div class="mb-3">
                        <label for="customCommand" class="form-label">输入命令</label>
                        <input type="text" class="form-control" id="customCommand" placeholder="例如: move_forward 100">
                    </div>
                    <button class="btn btn-primary w-100 mb-3" onclick="sendCustomCommand()">
                        <i class="bi bi-send"></i> 发送命令
                    </button>
                    
                    <h4><i class="bi bi-journal-text"></i> 操作日志</h4>
                    <div class="log-container" id="logContainer">
                        <!-- 日志内容将通过JavaScript动态添加 -->
                    </div>
                </div>
            </div>
        </div>
    </div>

    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js"></script>
    <script>
        let currentMode = 'manual';
        
        function toggleMode() {
            const modeSwitch = document.getElementById('modeSwitch');
            const autoMode = modeSwitch.checked;
            
            fetch('/api/mode', {
              method: 'POST',
              headers: {
                'Content-Type': 'application/json',
              },
              body: JSON.stringify({auto_mode: autoMode})
            })
            .then(response => response.json())
            .then(data => {
              if(data.status === 'success') {
                document.getElementById('manualControl').style.display = autoMode ? 'none' : 'block';
                document.getElementById('autoControl').style.display = autoMode ? 'block' : 'none';
                document.getElementById('modeText').textContent = autoMode ? '自动模式' : '手动模式';
                document.getElementById('modeIndicator').className = autoMode ? 
                  'mode-indicator mode-auto' : 'mode-indicator mode-manual';
              }
            });
        }

        function updateModeDisplay() {
            const modeText = document.getElementById('modeText');
            const modeIndicator = document.getElementById('modeIndicator');
            const modeSwitch = document.getElementById('modeSwitch');
            
            modeText.textContent = currentMode === 'auto' ? '自动模式' : '手动模式';
            modeIndicator.className = currentMode === 'auto' ? 'mode-indicator mode-auto' : 'mode-indicator mode-manual';
            modeSwitch.checked = currentMode === 'auto';
            
            document.getElementById('manualControl').style.display = currentMode === 'manual' ? 'block' : 'none';
            document.getElementById('autoControl').style.display = currentMode === 'auto' ? 'block' : 'none';
        }

        function sendCommand(command) {
            addLog('控制', `发送命令: ${command}`, 'info');
            fetch('/api/test', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ command: command })
            })
            .then(response => response.json())
            .then(data => {
                if (data.status === 'success') {
                    addLog('控制', `命令执行成功: ${command}`, 'success');
                    fetchStatus();
                } else {
                    addLog('控制', `命令执行失败: ${data.message}`, 'error');
                }
            })
            .catch(error => {
                addLog('控制', `命令发送失败: ${error.message}`, 'error');
            });
        }

        function sendCustomCommand() {
            const commandInput = document.getElementById('customCommand');
            const command = commandInput.value.trim();
            
            if (!command) {
                addLog('控制', '请输入有效的命令', 'warning');
                return;
            }
            
            sendCommand(command);
            commandInput.value = '';
        }

        function refreshStatus() {
            addLog('系统', '手动刷新状态', 'info');
            fetchStatus();
        }

        function addLog(source, message, type) {
            const logContainer = document.getElementById('logContainer');
            const logEntry = document.createElement('div');
            logEntry.className = 'log-entry';
            
            const timestamp = new Date().toLocaleTimeString();
            const typeClass = type === 'error' ? 'text-danger' : 
                            type === 'success' ? 'text-success' : 
                            type === 'warning' ? 'text-warning' : 'text-info';
            
            logEntry.innerHTML = `
                <span class="text-muted">[${timestamp}]</span>
                <span class="fw-bold">${source}:</span>
                <span class="${typeClass}">${message}</span>
            `;
            
            logContainer.prepend(logEntry);
            
            if (logContainer.children.length > 50) {
                logContainer.removeChild(logContainer.lastChild);
            }
        }

        function connectVideo() {
            const ip = document.getElementById('videoIP').value.trim();
            if(!ip) {
                addLog('视频', '请输入有效的视频流地址', 'warning');
                return;
            }
            
            const username = document.getElementById('videoUsername').value.trim();
            const password = document.getElementById('videoPassword').value.trim();
            const videoFeed = document.getElementById('videoFeed');
            const placeholder = document.getElementById('videoPlaceholder');
            
            // 构建带认证的视频流URL
            let videoUrl = ip;
            if(username && password) {
                // 处理海康等需要认证的视频流
                const protocol = ip.startsWith('rtsp') ? 'rtsp' : 'http';
                videoUrl = `${protocol}://${username}:${password}@${ip.replace(/^rtsp:\/\/|^http:\/\//, '')}`;
            }
            
            videoFeed.src = videoUrl;
            videoFeed.load();
            
            videoFeed.oncanplay = function() {
                videoFeed.style.display = 'block';
                placeholder.style.display = 'none';
                videoFeed.play();
                addLog('视频', `已连接到 ${ip}`, 'success');
            };
            
            videoFeed.onerror = function() {
                videoFeed.style.display = 'none';
                placeholder.style.display = 'block';
                addLog('视频', `连接失败: ${ip}`, 'error');
            };
        }

        function fetchStatus() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('hardwareStatus').textContent = data.system_status || '正常';
                    document.getElementById('cpuUsage').style.width = `${data.cpu_usage || 0}%`;
                    document.getElementById('cpuUsage').textContent = `${data.cpu_usage || 0}%`;
                    document.getElementById('voltage').textContent = data.voltage || '--';
                    document.getElementById('current').textContent = data.current || '--';
                    document.getElementById('depth').textContent = data.depth || '--';
                    document.getElementById('temperature').textContent = data.temperature || '--';
                });
        }

        function emergencyStop() {
            if(confirm('确定要执行紧急停止吗？这将立即停止所有电机！')) {
                sendCommand('emergency_stop');
                addLog('安全', '已触发紧急停止', 'danger');
            }
        }

        document.addEventListener('DOMContentLoaded', function() {
            fetchStatus();
            setInterval(fetchStatus, 3000);
            
            const videoFeed = document.getElementById('videoFeed');
            if(videoFeed.src) {
                setInterval(() => {
                    if(videoFeed.readyState === 4) {
                        videoFeed.load();
                    }
                }, 5000);
            }
        });
    </script>
</body>
</html>

<!-- 在标题后添加状态指示 -->
<div class="connection-status">
    <span id="wifiStatus" class="badge"></span>
</div>

<script>
// 添加WiFi状态检查
function checkWiFiStatus() {
    fetch('/api/status')
        .then(response => response.json())
        .then(data => {
            const statusEl = document.getElementById('wifiStatus');
            if(data.wifi_connected) {
                statusEl.className = 'badge bg-success';
                statusEl.textContent = `已连接: ${data.wifi_ssid}`;
            } else {
                statusEl.className = 'badge bg-danger';
                statusEl.textContent = '未连接WiFi';
            }
        });
}

// 初始化时和定期检查状态
document.addEventListener('DOMContentLoaded', function() {
    checkWiFiStatus();
    setInterval(checkWiFiStatus, 5000);
});
</script>

<script>
    // 新增：获取实时状态数据
    function fetchStatus() {
        fetch('/api/status')
            .then(response => response.json())
            .then(data => {
                // 更新硬件状态
                document.getElementById('hardwareStatus').textContent = data.system_status || '正常';
                document.getElementById('cpuUsage').style.width = `${data.cpu_usage || 0}%`;
                document.getElementById('cpuUsage').textContent = `${data.cpu_usage || 0}%`;
                
                // 更新动力系统
                document.getElementById('voltage').textContent = data.voltage || '--';
                document.getElementById('current').textContent = data.current || '--';
                
                // 更新环境数据
                document.getElementById('depth').textContent = data.depth || '--';
                document.getElementById('temperature').textContent = data.temperature || '--';
            });
    }

    // 新增：紧急停止功能
    function emergencyStop() {
        if(confirm('确定要执行紧急停止吗？这将立即停止所有电机！')) {
            sendCommand('emergency_stop');
            addLog('安全', '已触发紧急停止', 'danger');
        }
    }

    // 修改：初始化时自动获取状态
    document.addEventListener('DOMContentLoaded', function() {
        checkWiFiStatus();
        fetchStatus(); // 新增
        setInterval(checkWiFiStatus, 5000);
        setInterval(fetchStatus, 3000); // 新增：每3秒自动刷新状态
    });
</script>

<!-- 删除重复的结束标签和脚本 -->
</div>  <!-- 控制模式模块结束 -->

)rawliteral";

#endif