import { defineConfig } from 'vite';

// REPLACE WITH YOUR ESP32's ACTUAL IP

export default defineConfig({
  server: {
    proxy: {
      '/api': {
        target: 'http://172.20.10.7',
        changeOrigin: true,
        rewrite: (path) => path.replace(/^\/api/, ''),
      },
    },
  },
});