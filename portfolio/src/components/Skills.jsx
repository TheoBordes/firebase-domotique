import { motion } from 'framer-motion';
import {
  FiCpu,
  FiCode,
  FiSettings,
  FiTool,
} from 'react-icons/fi';

const skillCategories = [
  {
    title: "Électronique",
    icon: <FiCpu size={24} />,
    skills: ["Conception de circuits", "PCB Design", "FPGA", "Microcontrôleurs", "Arduino", "Raspberry Pi"]
  },
  {
    title: "Programmation",
    icon: <FiCode size={24} />,
    skills: ["C/C++", "Python", "VHDL", "Ladder", "Assembly", "JavaScript"]
  },
  {
    title: "Automatisme",
    icon: <FiSettings size={24} />,
    skills: ["Automates (API)", "GRAFCET", "Supervision", "Régulation", "Capteurs", "Actionneurs"]
  },
  {
    title: "Outils",
    icon: <FiTool size={24} />,
    skills: ["Altium Designer", "Proteus", "LabVIEW", "Matlab", "Git", "Unity"]
  }
];

const Skills = () => {
  return (
    <section id="skills" className="section-padding">
      <div className="max-w-7xl mx-auto">
        <h2 className="text-3xl font-bold mb-12 text-center">Compétences</h2>
        
        <div className="grid md:grid-cols-2 lg:grid-cols-4 gap-8">
          {skillCategories.map((category, index) => (
            <motion.div
              key={index}
              initial={{ opacity: 0, y: 20 }}
              whileInView={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.5, delay: index * 0.1 }}
              viewport={{ once: true }}
              className="bg-primary/30 p-6 rounded-lg"
            >
              <div className="text-secondary mb-4">{category.icon}</div>
              <h3 className="text-xl font-semibold mb-4">{category.title}</h3>
              <ul className="space-y-2">
                {category.skills.map((skill, skillIndex) => (
                  <li
                    key={skillIndex}
                    className="text-textSecondary flex items-center gap-2"
                  >
                    <span className="w-2 h-2 bg-secondary rounded-full"></span>
                    {skill}
                  </li>
                ))}
              </ul>
            </motion.div>
          ))}
        </div>
      </div>
    </section>
  );
};

export default Skills;
