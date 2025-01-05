import { motion } from 'framer-motion';
import { FiMail, FiPhone, FiMapPin } from 'react-icons/fi';

const Contact = () => {
  const handleSubmit = (e) => {
    e.preventDefault();
    // Ajoutez ici la logique d'envoi du formulaire
  };

  return (
    <section id="contact" className="section-padding">
      <div className="max-w-7xl mx-auto">
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.5 }}
          viewport={{ once: true }}
        >
          <h2 className="text-3xl font-bold mb-12 text-center">Contact</h2>
          
          <div className="grid md:grid-cols-2 gap-12">
            <div>
              <h3 className="text-xl font-semibold mb-6">Contactez-moi</h3>
              <p className="text-textSecondary mb-8">
                N'hésitez pas à me contacter pour discuter de vos projets ou
                opportunités de collaboration.
              </p>
              
              <div className="space-y-4">
                <div className="flex items-center gap-4 text-textSecondary">
                  <FiMail className="text-secondary" size={20} />
                  <span>votre@email.com</span>
                </div>
                <div className="flex items-center gap-4 text-textSecondary">
                  <FiPhone className="text-secondary" size={20} />
                  <span>+33 6 XX XX XX XX</span>
                </div>
                <div className="flex items-center gap-4 text-textSecondary">
                  <FiMapPin className="text-secondary" size={20} />
                  <span>Votre Ville, France</span>
                </div>
              </div>
            </div>
            
            <form onSubmit={handleSubmit} className="space-y-6">
              <div>
                <label htmlFor="name" className="block text-sm font-medium mb-2">
                  Nom
                </label>
                <input
                  type="text"
                  id="name"
                  name="name"
                  required
                  className="w-full px-4 py-2 rounded bg-primary/30 border border-textSecondary/20 focus:border-secondary outline-none transition-colors"
                />
              </div>
              
              <div>
                <label htmlFor="email" className="block text-sm font-medium mb-2">
                  Email
                </label>
                <input
                  type="email"
                  id="email"
                  name="email"
                  required
                  className="w-full px-4 py-2 rounded bg-primary/30 border border-textSecondary/20 focus:border-secondary outline-none transition-colors"
                />
              </div>
              
              <div>
                <label htmlFor="message" className="block text-sm font-medium mb-2">
                  Message
                </label>
                <textarea
                  id="message"
                  name="message"
                  rows="4"
                  required
                  className="w-full px-4 py-2 rounded bg-primary/30 border border-textSecondary/20 focus:border-secondary outline-none transition-colors"
                ></textarea>
              </div>
              
              <button
                type="submit"
                className="btn-primary w-full"
              >
                Envoyer
              </button>
            </form>
          </div>
        </motion.div>
      </div>
    </section>
  );
};

export default Contact;
