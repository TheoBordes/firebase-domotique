import { motion } from 'framer-motion';
import { FiGithub, FiExternalLink } from 'react-icons/fi';

const projects = [
  {
    title: "Robot suiveur de ligne",
    description: "Conception et réalisation d'un robot autonome capable de suivre une ligne noire sur fond blanc. Utilisation de capteurs infrarouges et d'un microcontrôleur Arduino.",
    image: "/robot-line-follower.jpg",
    tags: ["Arduino", "C++", "Électronique", "Capteurs"],
    github: "https://github.com/votre-projet-1",
    demo: "https://youtube.com/votre-demo-1"
  },
  {
    title: "Système domotique IoT",
    description: "Développement d'un système de contrôle domotique avec interface web, utilisant un Raspberry Pi et des capteurs pour monitorer la température, l'humidité et la luminosité.",
    image: "/home-automation.jpg",
    tags: ["Raspberry Pi", "Python", "IoT", "Web"],
    github: "https://github.com/votre-projet-2",
    demo: "https://youtube.com/votre-demo-2"
  },
  {
    title: "Bras robotique 3 axes",
    description: "Conception et programmation d'un bras robotique contrôlé par FPGA, avec interface utilisateur pour le contrôle des mouvements et la programmation de séquences.",
    image: "/robotic-arm.jpg",
    tags: ["VHDL", "FPGA", "Mécanique", "IHM"],
    github: "https://github.com/votre-projet-3",
    demo: "https://youtube.com/votre-demo-3"
  },
  {
    title: "Automate de tri de pièces",
    description: "Réalisation d'un système automatisé de tri de pièces métalliques par taille et matériau, utilisant un API Siemens et des capteurs inductifs.",
    image: "/sorting-system.jpg",
    tags: ["Automate", "GRAFCET", "Capteurs", "HMI"],
    github: "https://github.com/votre-projet-4",
    demo: "https://youtube.com/votre-demo-4"
  }
];

const Projects = () => {
  return (
    <section id="projects" className="section-padding">
      <div className="max-w-7xl mx-auto">
        <h2 className="text-3xl font-bold mb-12 text-center">Mes Projets</h2>
        
        <div className="grid md:grid-cols-2 gap-8">
          {projects.map((project, index) => (
            <motion.div
              key={index}
              initial={{ opacity: 0, y: 20 }}
              whileInView={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.5, delay: index * 0.1 }}
              viewport={{ once: true }}
              className="bg-primary/30 rounded-lg overflow-hidden"
            >
              <div className="relative aspect-video">
                <img
                  src={project.image}
                  alt={project.title}
                  className="w-full h-full object-cover"
                />
                <div className="absolute inset-0 bg-primary/60 opacity-0 hover:opacity-100 transition-opacity duration-300 flex items-center justify-center gap-4">
                  <a
                    href={project.github}
                    target="_blank"
                    rel="noopener noreferrer"
                    className="text-secondary hover:text-white transition-colors"
                  >
                    <FiGithub size={24} />
                  </a>
                  <a
                    href={project.demo}
                    target="_blank"
                    rel="noopener noreferrer"
                    className="text-secondary hover:text-white transition-colors"
                  >
                    <FiExternalLink size={24} />
                  </a>
                </div>
              </div>
              
              <div className="p-6">
                <h3 className="text-xl font-semibold mb-2">{project.title}</h3>
                <p className="text-textSecondary mb-4">{project.description}</p>
                <div className="flex flex-wrap gap-2">
                  {project.tags.map((tag, tagIndex) => (
                    <span
                      key={tagIndex}
                      className="text-secondary text-sm px-2 py-1 rounded-full border border-secondary"
                    >
                      {tag}
                    </span>
                  ))}
                </div>
              </div>
            </motion.div>
          ))}
        </div>
      </div>
    </section>
  );
};

export default Projects;
