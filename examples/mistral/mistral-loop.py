import onnxruntime_genai as og

print("Loading model...")

# The first argument is the name of the folder containing the model files
model = og.Model("example-models/mistral-7b-fp16-cuda", og.DeviceType.CUDA)
print("Model loaded")
tokenizer = og.Tokenizer(model)
print("Tokenizer created")

# Note: Prompt was auto-generated by Copilot. Visit `copilot.microsoft.com` to try it out.
prompts = ["""In the heart of the ancient forest, where sunlight filtered through the dense canopy, there stood an old oak tree. Its gnarled roots clung to the earth, and its branches reached skyward like arthritic fingers. The villagers whispered tales of the tree's magic—a guardian of secrets, a keeper of forgotten memories.
One moonlit night, as the wind whispered secrets through the leaves, a young girl named Elowen ventured into the forest. Her grandmother's stories echoed in her mind—the oak tree held the key to unlocking her family's past. Elowen carried a small leather-bound book, its pages filled with cryptic symbols and faded ink. It was her grandmother's journal, passed down through generations.
Elowen approached the ancient oak, its bark etched with symbols that seemed to pulse with energy. She pressed her palm against the rough surface, feeling a surge of warmth. The tree stirred, its branches rustling like a thousand whispered secrets. Elowen closed her eyes and whispered her grandmother's name.
Suddenly, the air shimmered, and the oak split open, revealing a hidden chamber within its trunk. Elowen stepped inside, her breath catching as she beheld the treasures within. Ancient scrolls, delicate vials of starlight, and a silver locket—all waiting to reveal their secrets.
As Elowen explored the chamber, she realized that the oak tree was more than a guardian; it was a bridge between worlds. Each item held a memory, a piece of her family's history. She unclasped the silver locket, and a vision flooded her mind—a woman with eyes like the forest, standing beneath the same oak tree, her laughter carried by the wind.
Elowen's heart raced. Who was this woman? What had she lost? And why had her grandmother kept these memories hidden? The answers lay within the ancient oak, waiting for Elowen to unravel their threads.
And so, under the moon's watchful gaze, Elowen vowed to uncover the truth. She would decipher the symbols, unlock the vials of starlight, and follow the whispers of the wind. For the old oak tree held not only her family's past but also the promise of a future filled with magic and wonder.
Elowen spent countless nights within the hidden chamber of the ancient oak. Each artifact whispered its story to her—a symphony of memories woven into the very fabric of the tree. The silver locket revealed a forbidden love, the starlight vials held dreams of distant constellations, and the scrolls bore forgotten spells.
As Elowen deciphered the cryptic symbols in her grandmother’s journal, she discovered that her family was more than mortal. They were guardians of the in-between, tasked with maintaining the delicate balance between the earthly realm and the mystical one. The oak tree served as their conduit, its roots reaching deep into the earth and its branches touching the stars.
One moonless night, Elowen traced her lineage back through the generations. She found her great-great-grandmother’s name—Seraphina—etched into the oldest scroll. Seraphina had been a seer, her eyes reflecting the secrets of the cosmos. She had danced with spirits, whispered to the wind, and fallen in love with a celestial being—a star that had descended to the forest.
The star’s name was Lyra, and their love was forbidden. The heavens wept when they embraced, and the earth trembled with longing. But fate was unkind. Lyra was bound to the sky, and Seraphina to the oak. Their union was a fragile thread connecting realms, and it threatened to unravel the very fabric of existence.
Elowen’s heart ached for her ancestors. She wondered if their love had been worth the sacrifice—their memories hidden within the oak, their whispers carried by the wind. She yearned to reunite them, to mend the fractured bond between earth and sky.
Guided by her grandmother’s journal, Elowen embarked on a quest. She climbed the highest peaks, seeking ancient altars where starlight met soil. She collected dewdrops from spiderwebs, tears shed by the moon. And at the heart of a forgotten glade, she found the Celestial Pool—a mirror reflecting the heavens.
Elowen dipped the silver locket into the pool, its surface rippling like stardust. She whispered Lyra’s name, and the water shimmered. A figure materialized—an ethereal being with eyes like galaxies. It was Lyra, still bound to the sky but yearning for Seraphina.
“Elowen,” Lyra’s voice echoed, “you carry the blood of seers. You can bridge our worlds.”
Elowen’s resolve hardened. She would reunite the lovers, even if it meant unraveling reality itself. She gathered the vials of starlight, the forgotten spells, and climbed the ancient oak. Its branches quivered, sensing her purpose.
At the highest bough, Elowen chanted the incantation—a melody woven from memories, love, and sacrifice. The oak split open, revealing a portal—a bridge of light connecting earth and sky. Elowen stepped through, her grandmother’s journal clutched to her chest.
In the celestial realm, she found Seraphina—a wisp of light, her eyes filled with longing. And there, across the expanse, was Lyra—a constellation burning bright.
Elowen held out the silver locket. “Love transcends boundaries,” she declared. “Let this be our legacy.”
The lovers reached for each other, their fingers brushing stardust. The oak trembled, and reality quivered. Elowen closed her eyes, feeling the threads of existence weave anew.
When she opened them, the ancient oak stood whole, its bark etched with new symbols—a testament to love’s enduring magic. And in the heart of the forest, sunlight filtered through the dense canopy, illuminating the gnarled roots and arthritic branches.
Elowen smiled. The whispers of the wind carried a new tale—one of sacrifice, reunion, and the unbreakable bond between earth and sky.
And so, the old oak tree remained—a guardian of secrets, a keeper of forgotten memories, and a bridge between worlds.
In the heart of the bustling city, where neon signs flickered and commuters hurried along crowded sidewalks, there existed a hidden café. Its entrance was unassuming—a weathered wooden door tucked between a high-rise office building and a vintage record store. The sign above read "The Midnight Brew," its letters painted in midnight blue.
Those who stumbled upon the café often did so by accident. Perhaps they took a wrong turn while searching for the latest trendy coffee shop or lost track of time during a late-night walk. But once inside, they discovered a world beyond the ordinary.
The interior was a blend of nostalgia and whimsy. Velvet curtains framed the windows, casting a soft glow on mismatched tables and chairs. The walls were adorned with antique clocks, each showing a different time—some ticking backward, others frozen at midnight. The air smelled of freshly ground coffee beans and secrets waiting to be shared.
The patrons were an eclectic mix—a jazz pianist with ink-stained fingers, a librarian who whispered poetry to her cup, a time traveler nursing a cup of chamomile tea. The barista, a mysterious figure named Elias, wore a bow tie and a perpetual smile. His eyes held stories—of lost love, forgotten dreams, and the passage of centuries.
The menu was equally enchanting. Instead of lattes and cappuccinos, it offered Moonlight Elixirs and Stardust Brews. Each drink had a purpose—a memory revival, a glimpse into parallel worlds, or simply a moment of solace. Regulars knew to order the Timeless Espresso when seeking answers, and the Galaxy Latte when yearning for adventure.
But the true magic of The Midnight Brew lay in its back room—a door marked "Beyond." Few dared to enter, for it led to realms unknown. Some claimed it opened to a moonlit forest, others to a starship hurtling through constellations. Elias would merely smile and say, "Choose your path wisely."
One stormy night, a young woman named Luna pushed open the door. She stepped into a swirling vortex of colors—a kaleidoscope of memories and possibilities. The door closed behind her, and she found herself standing on the edge of a cliff, overlooking a sea of silver mist.
Luna had a choice—to return to the café or explore the unknown. She thought of her mundane life—the deadlines, the routines, the unanswered questions. And then she thought of the jazz pianist's melody, the librarian's whispered verses, and Elias's enigmatic gaze.
With a deep breath, Luna stepped forward, her heart pounding. The mist enveloped her, and she vanished from The Midnight Brew. But her story remained—a legend whispered among the patrons, a reminder that sometimes, the most extraordinary journeys begin with a simple cup of coffee.
Luna stumbled through the swirling mist, her senses reeling. Colors danced around her—cobalt blues, iridescent greens, and shades she couldn’t name. The ground beneath her feet felt both solid and insubstantial, like stepping on moonbeams.
Ahead, a path materialized—a narrow trail bordered by luminous mushrooms. Their soft glow illuminated the way, revealing a forest unlike any Luna had seen. Trees stood tall, their trunks adorned with silver leaves that rustled in a melody only she could hear. Birds with wings of stardust flitted between branches, their songs echoing through the enchanted air.
Luna’s heart raced. She had chosen the unknown, and now she was part of a story woven into the fabric of existence. But what was her purpose here? Why had The Midnight Brew led her to this place?
As she walked, Luna noticed that time flowed differently. Minutes stretched into hours, yet she felt neither hunger nor fatigue. She touched the bark of a tree, and images flooded her mind—a cosmic library where books whispered secrets, a moonlit ballroom where forgotten dances unfolded, and a mirror pool reflecting alternate versions of herself.
The forest held answers, but it also posed questions. Luna encountered other travelers—a star-cloaked wanderer who spoke in riddles, a shadow weaver who wove memories into tapestries, and a timekeeper with eyes like ancient galaxies. Each offered cryptic guidance, urging her to follow her intuition.
One day, Luna reached a clearing—a Celestial Grove where constellations hung low, their patterns shifting. In the center stood a silver pedestal, upon which rested a single crystal goblet filled with shimmering liquid. The goblet seemed to hold the essence of the universe itself.
The star-cloaked wanderer appeared beside her. “Drink,” he said, his voice echoing through the grove. “Choose your destiny.”
Luna hesitated. The liquid sparkled like memories—of her childhood dreams, her lost love, and the unanswered questions that had led her to The Midnight Brew. She dipped her fingers into the goblet, and images swirled—a forgotten lullaby, a doorway of mirrors, and Elias’s enigmatic smile.
“Drink,” the wanderer repeated. “The choice is yours.”
Luna lifted the goblet to her lips. The liquid tasted like moonlight and longing. Visions flooded her—a reunion with her grandmother, the jazz pianist’s melody leading her home, and a final glimpse of The Midnight Brew.
She understood now. The café was a nexus—a place where souls intersected, where time looped backward, and where choices rippled across dimensions. Elias, the barista, was more than he seemed—a guardian of paths, a keeper of choices.
Luna drank deeply, her mind expanding. She saw the threads connecting her mundane life to the cosmic tapestry—the deadlines, the routines, the unanswered questions—all woven into something greater. She glimpsed Seraphina and Lyra, their love transcending realms, and knew that her journey was intertwined with theirs.
When Luna opened her eyes, she stood once again at the entrance of The Midnight Brew. The wooden door beckoned, and Elias awaited behind the counter, his eyes filled with knowing.
“Welcome back,” he said, pouring a cup of Moonlight Elixir. “What did you discover?”
Luna smiled. “That every cup of coffee holds a universe,” she replied. “And that sometimes, the most extraordinary journeys begin with a simple choice.”
And so, Luna returned to her mundane life, but she carried the forest within her—a map of constellations, a taste of moonlight, and the promise of more adventures. The Midnight Brew remained, its wooden door inviting wanderers, dreamers, and those seeking answers.
And somewhere, beyond the veil, Seraphina and Lyra danced—a love story etched into the cosmos, whispered by the wind, and brewed in the heart of midnight.
In a quiet coastal town, where seagulls wheeled overhead and salt-laden breezes carried whispers of forgotten tales, there stood an abandoned lighthouse. Its white paint had peeled, revealing weathered wood beneath. The lantern room, once a beacon guiding ships safely home, now sat empty.
The townspeople spoke of the lighthouse in hushed tones. They said it was haunted—that on moonless nights, a spectral figure could be seen climbing the winding staircase, its footsteps echoing through the hollow tower.
One stormy evening, a young artist named Evelyn arrived in the town. She carried a canvas, her brushes, and a heart heavy with grief. Her sister, a sailor, had vanished at sea, leaving behind only a cryptic note: “Find the light in the darkness.”
Driven by love and desperation, Evelyn sought answers. She wandered the rocky shore, her eyes drawn to the abandoned lighthouse. Its windows were like empty eyes, staring out at the churning waves.
As the moon rose, Evelyn climbed the narrow staircase. Cobwebs clung to her fingers, and the air smelled of salt and decay. At the top, she pushed open the lantern room door. The wind howled, and rain splattered against the glass.
And there, in the center of the room, stood a flickering candle. Its flame danced, casting shadows on the walls. Evelyn approached, her heart racing.
Was this the light her sister had spoken of? Or was it something more—a connection between realms, a bridge to the lost?
Evelyn dipped her brush into the candle’s flame and painted. The canvas absorbed the light, revealing images—a ship caught in a tempest, a sailor clinging to a broken mast, and a lighthouse standing firm.
As the last stroke fell, the spectral figure materialized—a woman with seafoam eyes and hair like tangled seaweed. She reached out, her touch both icy and warm.
“You seek answers,” the ghost whispered. “I am the keeper of memories—the one who guides lost souls.”
Evelyn’s sister appeared beside her, translucent and smiling. “Find the light,” she said. “It leads to hope, to redemption.”
And so, Evelyn vowed to restore the lighthouse. She scraped away the peeling paint, revealing the wood’s hidden strength. She lit the lantern, and its beam cut through the storm.
The townspeople watched in awe as the lighthouse blazed to life. Ships altered course, and sailors found safe harbor.
But Evelyn knew the true purpose—the light not only guided ships but also bridged the gap between the living and the lost. And as she painted, her sister’s laughter echoed through the tower.
For in that quiet coastal town, where seagulls wheeled overhead and salt-laden breezes carried whispers, hope flickered like a candle in the darkness.
Evelyn stood in the lantern room, her brush still warm from the candle’s flame. The spectral figure—the keeper of memories—watched her with eyes that held both sorrow and purpose.
“You seek answers,” the ghost whispered again. “But answers come at a cost.”
Evelyn’s sister, translucent and radiant, stepped closer. “The lighthouse,” she said, “it’s more than a guide for ships. It’s a bridge—a threshold between realms.”
The storm outside intensified, rain lashing against the glass. The sea roared, hungry for lost souls. Evelyn’s heart clenched. She had painted her sister back into existence, but what did it mean? What was the purpose of this spectral reunion?
“Tell me,” Evelyn pleaded. “Why did you vanish? Why leave only a cryptic note?”
Her sister’s laughter echoed—a melody woven into the wind. “Because,” she said, “I found the light.”
Evelyn’s mind raced. The candle, the canvas, the hidden purpose—the pieces clicked together. Her sister had glimpsed something beyond life, something that transcended the mundane. And now, Evelyn stood on the precipice of understanding.
The ghost beckoned. “The lighthouse must be restored,” she said. “Its light binds worlds. But to do so, you must retrieve the lost memories—the fragments scattered across time.”
Evelyn nodded, her resolve firm. “Where do I begin?”
“Within,” the ghost replied. “The lighthouse’s heart.”
Evelyn descended the winding staircase, her footsteps echoing. In the base of the tower, she found a hidden chamber—a Memory Vault. Its shelves held crystal orbs, each containing a fragment of forgotten lives—the laughter of children, the scent of blooming roses, the taste of salt on sun-kissed skin.
She touched an orb, and memories flooded her—the warmth of her sister’s hand, the way they’d danced in rainstorms, the promise to find the light. But other memories surfaced too—of betrayal, of choices made in darkness.
Evelyn collected the orbs, her canvas absorbing their essence. She painted—the ship caught in the tempest, the sailor clinging to the mast, and the lighthouse standing firm. The spectral figure watched, her eyes filled with longing.
“Release them,” the ghost urged. “Let the memories guide you.”
Evelyn stepped outside, the storm still raging. She climbed to the lantern room, her canvas glowing. The lighthouse blazed anew, its beam cutting through rain and mist. Ships altered course, sailors found safe harbor, and lost memories danced in the light.
But Evelyn knew her sister’s truth—the light wasn’t just for ships. It was a beacon for souls—the living and the departed. She touched the spectral figure’s hand, and they merged—a bridge between realms.
The townspeople marveled at the restored lighthouse. They spoke of its magic, its ability to heal hearts and guide lost wanderers. And Evelyn? She painted—capturing sunsets, moonrise, and the whispers of the sea.
One moonless night, she climbed to the lantern room. Her sister stood beside her, both solid and ethereal. “Why?” Evelyn asked. “Why did you find the light?”
Her sister smiled. “To remind you,” she said, “that love transcends time. That memories are threads connecting us all.”
And so, the lighthouse stood—a sentinel of memories, a bridge between realms, and a testament to the light found in the darkest corners of existence.
"""]

input_tokens = tokenizer.encode_batch(prompts)

params = og.GeneratorParams(model)
params.set_search_options({"max_length": input_tokens.shape[1] + 256, "repetition_penalty": 3.0})
params.input_ids = input_tokens


generator = og.Generator(model, params)

print("Generator created")

print("Running generation loop ...")

while not generator.is_done():
    generator.compute_logits()
    generator.generate_next_token_top()

print("Outputs:")

for i in range(len(prompts)):
   print(tokenizer.decode(generator.get_sequence(i)))
   print()
