import { Routes, Route } from 'react-router-dom'
import Home from './page/Home.tsx'
import About from './page/About.tsx'

function App() {
  return (
    <Routes>
      <Route path="/" element={<Home />} />
      <Route path="/about" element={<About />} />
    </Routes>
  )
}

export default App
