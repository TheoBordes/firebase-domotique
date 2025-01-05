/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        primary: "#0f172a",     // Fond principal (slate-900)
        secondary: "#38bdf8",   // Accent bleu moderne
        tertiary: "#f0f9ff",    // Bleu très clair
        textPrimary: "#f1f5f9", // Texte principal
        textSecondary: "#94a3b8", // Texte secondaire
      },
      fontFamily: {
        sans: ['Plus Jakarta Sans', 'sans-serif'], // Police moderne
        mono: ['JetBrains Mono', 'monospace'],     // Police pour le code
      },
      boxShadow: {
        'soft': '0 0 20px rgba(56, 189, 248, 0.15)',
      },
    },
  },
  plugins: [],
}
