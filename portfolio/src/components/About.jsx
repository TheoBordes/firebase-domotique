import { motion } from 'framer-motion';

const About = () => {
  return (
    <section id="about" className="section-padding">
      <div className="max-w-7xl mx-auto">
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.5 }}
          viewport={{ once: true }}
          className="grid md:grid-cols-2 gap-8 items-center"
        >
          <div>
            <h2 className="text-3xl font-bold mb-6">À propos de moi</h2>
            <div className="space-y-4 text-textSecondary">
              <p>
                Étudiant passionné en Génie Électrique et Informatique Industrielle (GEII),
                je suis fasciné par l'intersection entre l'électronique, l'automatisation et
                l'informatique.
              </p>
              <p>
                Mon parcours m'a permis de développer des compétences variées, de la conception
                de circuits électroniques à la programmation de microcontrôleurs, en passant
                par l'automatisation industrielle et le développement d'interfaces homme-machine.
              </p>
              <p>
                Je suis particulièrement intéressé par la robotique, l'IoT et l'industrie 4.0.
                Mon objectif est de contribuer à l'innovation technologique en combinant mes
                connaissances en électronique et en programmation.
              </p>
              <p>
                En dehors de mes études, je participe à des projets personnels et des
                compétitions de robotique pour approfondir mes connaissances et rester
                à la pointe de la technologie.
              </p>
            </div>
          </div>
          
          <div className="relative">
            <div className="aspect-square rounded-lg overflow-hidden">
              <img
                src="/your-image.jpg"
                alt="Portrait"
                className="w-full h-full object-cover"
              />
              <div className="absolute inset-0 border-2 border-secondary rounded-lg -z-10 translate-x-4 translate-y-4"></div>
            </div>
          </div>
        </motion.div>
      </div>
    </section>
  );
};

export default About;
