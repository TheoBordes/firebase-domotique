import { motion } from 'framer-motion';
import { HiArrowNarrowRight } from 'react-icons/hi';
import { BsGithub, BsLinkedin } from 'react-icons/bs';
import { HiOutlineMail } from 'react-icons/hi';

const Hero = () => {
  return (
    <section className="min-h-screen flex items-center section-padding relative overflow-hidden">
      {/* Fond avec gradient subtil */}
      <div className="absolute inset-0 bg-gradient-to-br from-primary via-primary to-primary/90" />
      
      <div className="max-w-4xl mx-auto w-full relative">
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.6 }}
          className="text-center"
        >
          <h2 className="text-secondary font-mono text-lg mb-4">
            Bonjour, je suis
          </h2>
          
          <h1 className="text-5xl sm:text-6xl lg:text-7xl font-bold mb-6">
            <span className="gradient-text">Ingénieur GEII</span>
          </h1>
          
          <h3 className="text-2xl sm:text-3xl text-textSecondary mb-8">
            Électronique & Informatique Industrielle
          </h3>
          
          <p className="text-textSecondary max-w-2xl mx-auto mb-8 text-lg">
            Passionné par le développement de solutions innovantes 
            à l'intersection de l'électronique et de l'informatique.
          </p>
          
          <div className="flex flex-wrap gap-6 justify-center mb-12">
            <a href="#projects" className="btn-primary inline-flex items-center group">
              Voir mes projets
              <HiArrowNarrowRight className="ml-2 group-hover:translate-x-1 transition-transform" />
            </a>
            <a href="#contact" className="btn-primary inline-flex items-center">
              Me contacter
            </a>
          </div>

          {/* Liens sociaux */}
          <div className="flex justify-center gap-8">
            <a
              href="https://github.com/votre-github"
              target="_blank"
              rel="noopener noreferrer"
              className="text-textSecondary hover:text-secondary transition-all duration-300"
            >
              <BsGithub size={24} />
            </a>
            <a
              href="https://linkedin.com/in/votre-linkedin"
              target="_blank"
              rel="noopener noreferrer"
              className="text-textSecondary hover:text-secondary transition-all duration-300"
            >
              <BsLinkedin size={24} />
            </a>
            <a
              href="mailto:votre@email.com"
              className="text-textSecondary hover:text-secondary transition-all duration-300"
            >
              <HiOutlineMail size={26} />
            </a>
          </div>
        </motion.div>
      </div>
    </section>
  );
};

export default Hero;
