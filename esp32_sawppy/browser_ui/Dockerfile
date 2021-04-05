FROM node:14
WORKDIR /browser_ui
COPY package.json yarn.lock /browser_ui
RUN yarn install
CMD ["yarn", "run", "start"]
